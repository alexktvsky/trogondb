#include "trogondb/connection_state.h"

#include <string>
#include <algorithm>

#include "trogondb/log/log_manager.h"
#include "trogondb/utils.h"

// TODO: Remove me
#include "trogondb/cmd/ping_command.h"
#include "trogondb/cmd/echo_command.h"
#include "trogondb/cmd/get_command.h"
#include "trogondb/cmd/set_command.h"

namespace trogondb {

// TODO: Move function to another file
cmd::CommandResult executeCommand(const std::string &commandName, const std::vector<std::string> &args)
{
    std::unique_ptr<cmd::ICommand> cmd;

    if (commandName == "ping") {
        cmd = std::make_unique<cmd::PingCommand>();
    }
    if (commandName == "echo" && args.size() == 1) {
        cmd = std::make_unique<cmd::EchoCommand>(args[0]);
    }
    // if (commandName == "get" && args.size() == 1) {
    //     cmd = std::make_unique<cmd::GetCommand>(m_store, args[0]);
    // }
    // if ((commandName == "set") && (args.size() == 2 || args.size() == 4)) {
    //     cmd = std::make_unique<cmd::SetCommand>(m_store, args);
    // }
    else {
        return cmd::CommandResult::error("-ERR unknown command\r\n");
    }

    return cmd->execute();
}

IConnectionState::IConnectionState(std::shared_ptr<Connection> connection)
    : m_connection(connection)
    , m_logger(log::LogManager::instance().getDefaultLogger())
{}

void IConnectionState::doRead(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred)
{}

void IConnectionState::doWrite(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred)
{}

void IConnectionState::doTimeout()
{}



void ReadingHeaderState::doRead(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred)
{
    buffer->commit(bytesTransferred);

    std::string data(boost::asio::buffers_begin(buffer->data()),
                     boost::asio::buffers_begin(buffer->data()) + bytesTransferred);

    size_t pos = data.find("\r\n");
    if (pos == std::string::npos) {
        m_connection->doRead();
        return;
    }

    size_t bytesConsumed = pos + 2;

    if (data[0] != '*') {
        m_logger->error("Failed ReadingHeaderState::doRead(): expected '*'");
        m_connection->changeState(std::make_shared<ErrorState>(m_connection, "Failed to ...")); // TODO
        m_connection->doWrite();
        return;
    }

    m_connection->m_context.expectedArgsCount = std::stoi(data.substr(1, pos)); // skip '*'
    m_logger->debug("ReadingHeaderState::doRead() expectedArgsCount: {}", m_connection->m_context.expectedArgsCount);

    buffer->consume(bytesConsumed);

    m_connection->changeState(std::make_shared<ReadingArgumentLengthState>(m_connection));

    if (bytesTransferred > bytesConsumed) {
        m_connection->m_state->doRead(buffer, bytesTransferred - bytesConsumed);
        return;
    }

    m_connection->doRead();
}

void ReadingArgumentLengthState::doRead(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred)
{
    buffer->commit(bytesTransferred);

    std::string data(boost::asio::buffers_begin(buffer->data()),
                     boost::asio::buffers_begin(buffer->data()) + bytesTransferred);

    size_t pos = data.find("\r\n");
    if (pos == std::string::npos) {
        m_connection->doRead();
        return;
    }

    size_t bytesConsumed = pos + 2;

    if (data[0] != '$') {
        m_logger->error("Failed ReadingArgumentLengthState::doRead(): expected '$'");
        m_connection->changeState(std::make_shared<ErrorState>(m_connection, "Failed to ...")); // TODO
        m_connection->doWrite();
        return;
    }

    m_connection->m_context.expectedNextBulkLength = std::stoi(data.substr(1, pos)); // skip '$'
    m_logger->debug("ArgumentLengthStateState::doRead() expectedNextBulkLength: {}", m_connection->m_context.expectedNextBulkLength);

    buffer->consume(bytesConsumed);

    m_connection->changeState(std::make_shared<ReadingArgumentState>(m_connection));

    if (bytesTransferred > bytesConsumed) {
        m_connection->m_state->doRead(buffer, bytesTransferred - bytesConsumed);
        return;
    }

    m_connection->doRead();
}

void ReadingArgumentState::doRead(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred)
{
    buffer->commit(bytesTransferred);

    std::string data(boost::asio::buffers_begin(buffer->data()),
                     boost::asio::buffers_begin(buffer->data()) + bytesTransferred);

    size_t pos = data.find("\r\n");
    if (pos == std::string::npos) {
        m_connection->doRead();
        return;
    }

    size_t bytesConsumed = pos + 2;

    std::string bulk = data.substr(0, pos);
    m_logger->debug("ReadingArgumentState::doRead() bulk: {}", bulk);

    if (bulk.length() != m_connection->m_context.expectedNextBulkLength) {
        m_logger->error("Failed to ReadingArgumentState::doRead(): length of '{}' ({}) does not match expected length ({})", bulk, bulk.size(), m_connection->m_context.expectedNextBulkLength);
        m_connection->changeState(std::make_shared<ErrorState>(m_connection, "Failed to ...")); // TODO
        m_connection->doWrite();
    }

    // First bulk is a name of command
    if (m_connection->m_context.cmd.length() == 0) {
        m_connection->m_context.cmd = stringToLower(bulk);
    }
    else {
        m_connection->m_context.args.push_back(bulk);
    }

    buffer->consume(bytesConsumed);

    m_connection->changeState(std::make_shared<ReadingArgumentLengthState>(m_connection));

    if (bytesTransferred > bytesConsumed) {
        m_connection->m_state->doRead(buffer, bytesTransferred - bytesConsumed);
        return;
    }

    if (m_connection->m_context.args.size() == m_connection->m_context.expectedArgsCount - 1) {
        m_logger->debug("Executing command '{}'", m_connection->m_context.cmd);
        cmd::CommandResult result = executeCommand(m_connection->m_context.cmd, m_connection->m_context.args);
        if (result.ok) {
            m_connection->changeState(std::make_shared<WritingResponseState>(m_connection, result.output));
            m_connection->doWrite();
        }
        else {
            m_connection->changeState(std::make_shared<ErrorState>(m_connection, fmt::format("Failed to ...: {}", result.output)));
            m_connection->doWrite();
        }
    }
    else {
        m_connection->doRead();
    }
}

ErrorState::ErrorState(std::shared_ptr<Connection> connection, const std::string &message)
    : IConnectionState(connection)
    , m_message(message)
{}

void ErrorState::doWrite(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred)
{
    size_t size = m_message.size();
    auto outputBuffer = buffer->prepare(size);

    std::copy(m_message.begin(), m_message.end(), boost::asio::buffer_cast<unsigned char*>(outputBuffer));

    buffer->commit(size);

    // m_connection->cancel();
}

WritingResponseState::WritingResponseState(std::shared_ptr<Connection> connection, const std::string &output)
    : IConnectionState(connection)
    , m_output(output)
{}

void WritingResponseState::doWrite(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred)
{

}

} // namespace trogondb
