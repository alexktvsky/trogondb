#include "trogondb/session.h"

#include <fmt/format.h>

#include "trogondb/command/ping_command.h"
#include "trogondb/command/echo_command.h"
#include "trogondb/command/get_command.h"
#include "trogondb/command/set_command.h"
#include "trogondb/exception.h"
#include "trogondb/utils.h"

namespace trogondb {

Session::Session(boost::asio::ip::tcp::socket socket, const std::shared_ptr<Store> &store, const std::shared_ptr<logging::Logger> &logger)
    : m_socket(std::move(socket))
    , m_state(SessionState::WAITING_FOR_ARRAY_HEADER)
    , m_writeOffset(0)
    , m_expectedArgsCount(0)
    , m_argsRead(0)
    , m_expectedBulkLength(0)
    , m_timer(m_socket.get_executor())
    , m_store(store)
    , m_logger(logger)
{
    // ...
}

void Session::start()
{
    m_logger->info("New client connected {}", m_socket.remote_endpoint().address().to_string());
    doReadLine();
}

void Session::startTimeout()
{
    m_timer.expires_after(std::chrono::seconds(TIMEOUT_SECONDS));
    m_timer.async_wait(std::bind(&Session::onTimeout, shared_from_this(), std::placeholders::_1));
}

void Session::onTimeout(const boost::system::error_code &err)
{
    if (err) {
        return;
    }
    m_logger->info("Client idle, closing connection");
    m_state = SessionState::ERROR;
    m_socket.close();
    // m_server->removeSession(shared_from_this());
}

void Session::doReadLine()
{
    boost::asio::async_read_until(
        m_socket, m_readBuffer, "\r\n",
        std::bind(&Session::onReadLine, shared_from_this(), std::placeholders::_1, std::placeholders::_2));

    startTimeout();
}

void Session::onReadLine(const boost::system::error_code &err, size_t)
{
    if (err) {
        if (err == boost::asio::error::eof) {
            m_logger->info("Client closed connection");
        } else if (err == boost::asio::error::connection_reset) {
            m_logger->info("Client crashed or reset connection");
        } else {
            m_logger->error("Failed to onReadLine(): {}", err.message());
        }

        close();
        return;
    }

    std::istream is(&m_readBuffer);
    std::string line;
    std::getline(is, line);
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    if (line.empty()) {
        m_logger->error("Failed to onReadLine(): Empty line");
        close();
        return;
    }

    switch (m_state) {
        case SessionState::WAITING_FOR_ARRAY_HEADER:
            if (line[0] != '*') {
                m_logger->error("Failed to onReadLine(): Expected array");
                close();
                return;
            }
            m_expectedArgsCount = std::stoi(line.substr(1));
            m_argsRead = 0;
            m_parsedArgs.clear();
            m_state = SessionState::WAITING_FOR_BULK_LENGTH;
            break;
        case SessionState::WAITING_FOR_BULK_LENGTH:
            if (line[0] != '$') {
                m_logger->error("Failed to onReadLine(): Expected bulk length");
                close();
                return;
            }
            m_expectedBulkLength = std::stoi(line.substr(1));
            m_state = SessionState::WAITING_FOR_BULK_BODY;
            doReadBody();
            return;
        default:
            m_logger->error("Failed to onReadLine(): Invalid state in onReadLine()");
            close();
            return;
    }

    doReadLine();
}

void Session::doReadBody()
{
    size_t needed = m_expectedBulkLength + 2;
    if (m_readBuffer.size() >= needed) {
        onReadBody(boost::system::error_code(), needed);
        return;
    }

    boost::asio::async_read(
        m_socket, m_readBuffer, boost::asio::transfer_exactly(needed),
        std::bind(&Session::onReadBody, shared_from_this(), std::placeholders::_1, std::placeholders::_2));

    startTimeout();
}

void Session::onReadBody(const boost::system::error_code &err, size_t)
{
    if (err) {
        if (err == boost::asio::error::eof) {
            m_logger->info("Client closed connection");
        } else if (err == boost::asio::error::connection_reset) {
            m_logger->info("Client crashed or reset connection");
        } else {
            m_logger->error("Failed to onReadBody(): {}", err.message());
        }

        close();
        return;
    }

    std::istream is(&m_readBuffer);
    std::string data(m_expectedBulkLength, '\0');
    is.read(&data[0], m_expectedBulkLength);
    is.ignore(2); // \r\n
    m_parsedArgs.push_back(data);
    ++m_argsRead;

    if (m_argsRead < m_expectedArgsCount) {
        m_state = SessionState::WAITING_FOR_BULK_LENGTH;
        doReadLine();
    } else {
        m_state = SessionState::READY_TO_EXECUTE;
        executeCommand();
    }
}

std::unique_ptr<ICommand> Session::createCommand(const std::vector<std::string> &args)
{
    if (args.empty()) {
        return nullptr;
    }
    std::string cmd = stringToLower(args[0]);

    if (cmd == "ping") {
        return std::make_unique<PingCommand>();
    }
    if (cmd == "echo" && args.size() == 2) {
        return std::make_unique<EchoCommand>(args[1]);
    }
    if (cmd == "get" && args.size() == 2) {
        return std::make_unique<GetCommand>(m_store, args[1]);
    }
    if ((cmd == "set") && (args.size() == 3 || args.size() == 5)) {
        return std::make_unique<SetCommand>(m_store, args);
    }

    throw UnknowCommandException(fmt::format("Unknow command \'{}\'", cmd));
}

void Session::executeCommand()
{
    auto command = createCommand(m_parsedArgs);
    if (!command) {
        m_writeBuffer = "-ERR unknown or malformed command\r\n";
    } else {
        m_writeBuffer = command->execute();
    }

    m_writeOffset = 0;
    m_state = SessionState::WRITING_RESPONSE;
    doWrite();
}

void Session::doWrite()
{
    auto slice = boost::asio::buffer(m_writeBuffer.data() + m_writeOffset, m_writeBuffer.size() - m_writeOffset);

    boost::asio::async_write(
        m_socket, slice, std::bind(&Session::onWrite, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void Session::onWrite(const boost::system::error_code &err, size_t n)
{
    if (err) {
        if (err == boost::asio::error::eof) {
            m_logger->info("Client closed connection");
        } else if (err == boost::asio::error::connection_reset) {
            m_logger->info("Client crashed or reset connection");
        } else {
            m_logger->error("Failed to onWrite(): {}", err.message());
        }

        close();
        return;
    }

    m_writeOffset += n;
    if (m_writeOffset < m_writeBuffer.size()) {
        doWrite();
    } else {
        m_state = SessionState::WAITING_FOR_ARRAY_HEADER;
        doReadLine();
    }
}

void Session::close()
{
    m_timer.cancel();
    m_state = SessionState::ERROR;
    // m_server->removeSession(shared_from_this());
}

} // namespace trogondb
