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
    else if (commandName == "echo") {
        cmd = std::make_unique<cmd::EchoCommand>();
    }
    // else if (commandName == "get") {
    //     cmd = std::make_unique<cmd::GetCommand>(m_store);
    // }
    // else if (commandName == "set") {
    //     cmd = std::make_unique<cmd::SetCommand>(m_store);
    // }
    else {
        return cmd::CommandResult::error(fmt::format("unknown command '{}', with args beginning with: {}\r\n", commandName, args[0]));
    }

    return cmd->execute(args);
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
        m_logger->error("Failed ReadingHeaderState::doRead(): expected '*', got '{}'", data[0]);
        m_connection->changeState(std::make_shared<ErrorState>(m_connection, fmt::format("-ERR Protocol error: expected '*', got '{}'", data[0])));
        m_connection->m_state->doWrite(m_connection->m_writeBuffer, 0);
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
        m_logger->error("Failed ReadingArgumentLengthState::doRead(): expected '$', got '{}'", data[0]);
        m_connection->changeState(std::make_shared<ErrorState>(m_connection, fmt::format("-ERR Protocol error: expected '$', got '{}'", data[0])));
        m_connection->m_state->doWrite(m_connection->m_writeBuffer, 0);
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
        m_connection->changeState(std::make_shared<ErrorState>(m_connection, fmt::format("-ERR Protocol error: length of '{}' ({}) does not match expected length ({})", bulk, bulk.size(), m_connection->m_context.expectedNextBulkLength)));
        m_connection->m_state->doWrite(m_connection->m_writeBuffer, 0);
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
            m_connection->m_state->doWrite(m_connection->m_writeBuffer, 0);
        }
        else {
            m_connection->changeState(std::make_shared<ErrorState>(m_connection, fmt::format("-ERR {}", result.output)));
            m_connection->m_state->doWrite(m_connection->m_writeBuffer, 0);
        }
    }
    else {
        m_connection->doRead();
    }
}

ErrorState::ErrorState(std::shared_ptr<Connection> connection, const std::string &output)
    : IConnectionState(connection)
    , m_output(output)
{}

void ErrorState::doWrite(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred)
{
    m_logger->debug("ErrorState::doWrite() bytesTransferred: {}", bytesTransferred);

    if (bytesTransferred > 0) {
        m_output.erase(0, bytesTransferred);
    }

    if (m_output.size() == 0) {
        m_connection->changeState(std::make_shared<ClosedState>(m_connection));
        m_connection->close();
        return;
    }

    auto outputBuffer = buffer->prepare(m_output.size());

    std::copy(m_output.begin(), m_output.end(), boost::asio::buffer_cast<unsigned char*>(outputBuffer));

    buffer->commit(m_output.size());

    m_connection->doWrite();
}

WritingResponseState::WritingResponseState(std::shared_ptr<Connection> connection, const std::string &output)
    : IConnectionState(connection)
    , m_output(output)
{}

void WritingResponseState::doWrite(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred)
{
    m_logger->debug("WritingResponseState::doWrite() bytesTransferred: {}", bytesTransferred);

    if (bytesTransferred > 0) {
        m_output.erase(0, bytesTransferred);
    }

    if (m_output.size() == 0) {
        m_connection->changeState(std::make_shared<ClosedState>(m_connection));
        m_connection->close();
        return;
    }

    auto outputBuffer = buffer->prepare(m_output.size());

    std::copy(m_output.begin(), m_output.end(), boost::asio::buffer_cast<unsigned char*>(outputBuffer));

    buffer->commit(m_output.size());

    m_connection->doWrite();
}

} // namespace trogondb
