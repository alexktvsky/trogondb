#include "trogondb/connection.h"

#include <fmt/format.h>

#include "trogondb/cmd/ping_command.h"
#include "trogondb/cmd/echo_command.h"
#include "trogondb/cmd/get_command.h"
#include "trogondb/cmd/set_command.h"
#include "trogondb/exception.h"
#include "trogondb/utils.h"

namespace trogondb {

Connection::Connection(boost::asio::ip::tcp::socket socket,
                       const std::shared_ptr<log::Logger> &logger)
    // : m_socket(std::move(socket))
    // , m_state(ConnectionState::WAITING_FOR_ARRAY_HEADER)
    // , m_writeOffset(0)
    // , m_expectedArgsCount(0)
    // , m_argsRead(0)
    // , m_expectedBulkLength(0)
    // , m_timer(m_socket.get_executor())
    // , m_store(store)
    // , m_logger(logger)
    // , m_cancelled(false)
{}

void Connection::start()
{
    // m_logger->info("New client connected {}", m_socket.remote_endpoint().address().to_string());
    // doReadLine();
}

// void Connection::startTimeout()
// {
    // m_timer.expires_after(std::chrono::seconds(TIMEOUT_SECONDS));
    // m_timer.async_wait(std::bind(&Connection::onTimeout, shared_from_this(), std::placeholders::_1));
// }

// void Connection::onTimeout(const boost::system::error_code &err)
// {
//     if (err) {
//         m_logger->error("Failed to onTimeout(): {}", err.message());
//     }

//     m_logger->info("Client idle, closing connection");
//     cancel();
// }

// void Connection::doReadLine()
// {
//     boost::asio::async_read_until(
//         m_socket,
//         m_readBuffer,
//         "\r\n",
//         std::bind(&Connection::onReadLine, shared_from_this(), std::placeholders::_1, std::placeholders::_2));

//     startTimeout();
// }

// void Connection::onReadLine(const boost::system::error_code &err, size_t /*unused*/)
// {
//     if (err) {
//         if (err == boost::asio::error::eof) {
//             m_logger->info("Client closed connection");
//         }
//         else if (err == boost::asio::error::connection_reset) {
//             m_logger->info("Client crashed or reset connection");
//         }
//         else {
//             m_logger->error("Failed to onReadLine(): {}", err.message());
//         }

//         cancel();
//         return;
//     }

//     std::istream is(&m_readBuffer);
//     std::string line;
//     std::getline(is, line);

//     if (!line.empty() && line.back() == '\r') {
//         line.pop_back();
//     }

//     if (line.empty()) {
//         m_logger->error("Failed to onReadLine(): Empty line");
//         cancel();
//         return;
//     }

//     switch (m_state) {
//     case ConnectionState::WAITING_FOR_ARRAY_HEADER:
//         if (line[0] != '*') {
//             m_logger->error("Failed to onReadLine(): Expected array");
//             cancel();
//             return;
//         }
//         m_expectedArgsCount = std::stoi(line.substr(1));
//         m_argsRead = 0;
//         m_parsedArgs.clear();
//         m_state = ConnectionState::WAITING_FOR_BULK_LENGTH;
//         break;
//     case ConnectionState::WAITING_FOR_BULK_LENGTH:
//         if (line[0] != '$') {
//             m_logger->error("Failed to onReadLine(): Expected bulk length");
//             cancel();
//             return;
//         }
//         m_expectedBulkLength = std::stoi(line.substr(1));
//         m_state = ConnectionState::WAITING_FOR_BULK_BODY;
//         doReadBody();
//         return;
//     default:
//         m_logger->error("Failed to onReadLine(): Invalid state");
//         cancel();
//         return;
//     }

//     doReadLine();
// }

// void Connection::doReadBody()
// {
//     size_t needed = m_expectedBulkLength + 2;
//     if (m_readBuffer.size() >= needed) {
//         onReadBody(boost::system::error_code(), needed);
//         return;
//     }

//     boost::asio::async_read(
//         m_socket,
//         m_readBuffer,
//         boost::asio::transfer_exactly(needed),
//         std::bind(&Connection::onReadBody, shared_from_this(), std::placeholders::_1, std::placeholders::_2));

//     startTimeout();
// }

// void Connection::onReadBody(const boost::system::error_code &err, size_t /*unused*/)
// {
//     if (err) {
//         if (err == boost::asio::error::eof) {
//             m_logger->info("Client closed connection");
//         }
//         else if (err == boost::asio::error::connection_reset) {
//             m_logger->info("Client crashed or reset connection");
//         }
//         else {
//             m_logger->error("Failed to onReadBody(): {}", err.message());
//         }

//         cancel();
//         return;
//     }

//     std::istream is(&m_readBuffer);
//     std::string data(m_expectedBulkLength, '\0');
//     is.read(data.data(), m_expectedBulkLength);
//     is.ignore(2); // \r\n
//     m_parsedArgs.push_back(data);
//     ++m_argsRead;

//     if (m_argsRead < m_expectedArgsCount) {
//         m_state = ConnectionState::WAITING_FOR_BULK_LENGTH;
//         doReadLine();
//     }
//     else {
//         m_state = ConnectionState::READY_TO_EXECUTE;
//         executeCommand();
//     }
// }

// std::optional<std::unique_ptr<cmd::ICommand>> Connection::createCommand(const std::string &cmd, const std::vector<std::string> &args)
// {
//     if (cmd == "ping") {
//         return std::make_unique<cmd::PingCommand>();
//     }
//     if (cmd == "echo" && args.size() == 1) {
//         return std::make_unique<cmd::EchoCommand>(args[0]);
//     }
//     if (cmd == "get" && args.size() == 1) {
//         return std::make_unique<cmd::GetCommand>(m_store, args[0]);
//     }
//     if ((cmd == "set") && (args.size() == 2 || args.size() == 4)) {
//         return std::make_unique<cmd::SetCommand>(m_store, args);
//     }

//     return std::nullopt;
// }

// void Connection::executeCommand()
// {
//     // TODO: Improve performace
//     std::string cmd = stringToLower(m_parsedArgs[0]);
//     m_parsedArgs.erase(m_parsedArgs.begin());

//     auto result = createCommand(cmd, m_parsedArgs);
//     if (result) {
//         auto command = std::move(result.value());
//         m_writeBuffer = command->execute();
//     }
//     else {
//         m_writeBuffer = "-ERR unknown or malformed command\r\n";
//     }

//     m_writeOffset = 0;
//     m_state = ConnectionState::WRITING_RESPONSE;
//     doWrite();
// }

// void Connection::doWrite()
// {
//     auto slice = boost::asio::buffer(m_writeBuffer.data() + m_writeOffset, m_writeBuffer.size() - m_writeOffset);

//     boost::asio::async_write(
//         m_socket,
//         slice,
//         std::bind(&Connection::onWrite, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
// }

// void Connection::onWrite(const boost::system::error_code &err, size_t n)
// {
//     if (err) {
//         if (err == boost::asio::error::eof) {
//             m_logger->info("Client closed connection");
//         }
//         else if (err == boost::asio::error::connection_reset) {
//             m_logger->info("Client crashed or reset connection");
//         }
//         else {
//             m_logger->error("Failed to onWrite(): {}", err.message());
//         }

//         cancel();
//         return;
//     }

//     m_writeOffset += n;
//     if (m_writeOffset < m_writeBuffer.size()) {
//         doWrite();
//     }
//     else {
//         m_state = ConnectionState::WAITING_FOR_ARRAY_HEADER;
//         doReadLine();
//     }
// }

void Connection::cancel()
{
    // if (m_cancelled) {
    //     return;
    // }
    // m_timer.cancel();
    // m_socket.cancel();
    // m_state = ConnectionState::ERROR;
    // // m_server->removeConnection(shared_from_this());
    // m_cancelled = true;
}

} // namespace trogondb
