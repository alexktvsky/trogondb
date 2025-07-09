#include "trogondb/connection_state.h"

#include <string>
#include <algorithm>

#include "trogondb/log/log_manager.h"
#include "trogondb/utils.h"

namespace trogondb {

IConnectionState::IConnectionState(std::weak_ptr<Connection> connection)
    : m_logger(log::LogManager::instance().getDefaultLogger())
    , m_connection(connection)
{}

void IConnectionState::doRead(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred)
{}

void IConnectionState::doWrite(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred)
{}

void IConnectionState::doTimeout()
{}

void ReadingHeaderState::doRead(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred)
{
    auto connection = m_connection.lock();
    if (!connection) {
        throw Exception("Failed to acquire Connection: weak_ptr expired");
    }

    buffer->commit(bytesTransferred);

    std::string_view data(static_cast<const char*>(buffer->data().data()), bytesTransferred);

    size_t pos = data.find("\r\n");
    if (pos == std::string::npos) {
        connection->doRead();
        return;
    }

    size_t bytesConsumed = pos + 2;

    if (data[0] != '*') {
        m_logger->error("Failed ReadingHeaderState::doRead(): expected '*', got '{}'", data[0]);
        connection->changeState(std::make_shared<ErrorState>(connection, fmt::format("-ERR Protocol error: expected '*', got '{}'", data[0])));
        connection->m_state->doWrite(connection->m_writeBuffer, 0);
        return;
    }

    std::string_view expectedArgsStr(data.data() + 1, pos); // skip '*'
    auto expectedArgsCount = stringToNumber<uint32_t>(expectedArgsStr);
    if (!expectedArgsCount) {
        m_logger->error("Failed to ReadingHeaderState::doRead(): invalid multibulk length");
        connection->changeState(std::make_shared<ErrorState>(connection, "-ERR Protocol error: invalid multibulk length"));
        connection->m_state->doWrite(connection->m_writeBuffer, 0);
        return;
    }

    connection->m_context.expectedArgsCount = expectedArgsCount.value();
    m_logger->debug("ReadingHeaderState::doRead() expectedArgsCount: {}", connection->m_context.expectedArgsCount);

    buffer->consume(bytesConsumed);

    connection->changeState(std::make_shared<ReadingArgumentLengthState>(connection));

    if (bytesTransferred > bytesConsumed) {
        connection->m_state->doRead(buffer, bytesTransferred - bytesConsumed);
        return;
    }

    connection->doRead();
}

void ReadingArgumentLengthState::doRead(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred)
{
    auto connection = m_connection.lock();
    if (!connection) {
        throw Exception("Failed to acquire Connection: weak_ptr expired");
    }

    buffer->commit(bytesTransferred);

    std::string_view data(static_cast<const char*>(buffer->data().data()), bytesTransferred);

    size_t pos = data.find("\r\n");
    if (pos == std::string::npos) {
        connection->doRead();
        return;
    }

    size_t bytesConsumed = pos + 2;

    if (data[0] != '$') {
        m_logger->error("Failed ReadingArgumentLengthState::doRead(): expected '$', got '{}'", data[0]);
        connection->changeState(std::make_shared<ErrorState>(connection, fmt::format("-ERR Protocol error: expected '$', got '{}'", data[0])));
        connection->m_state->doWrite(connection->m_writeBuffer, 0);
        return;
    }

    std::string_view bulkLengthStr(data.data() + 1, pos); // skip '$'
    auto bulkLength = stringToNumber<uint32_t>(bulkLengthStr);
    if (!bulkLength) {
        m_logger->error("Failed to ReadingArgumentLengthState::doRead(): invalid bulk length");
        connection->changeState(std::make_shared<ErrorState>(connection, "-ERR Protocol error: invalid bulk length"));
        connection->m_state->doWrite(connection->m_writeBuffer, 0);
        return;
    }

    connection->m_context.expectedNextBulkLength = bulkLength.value();
    m_logger->debug("ArgumentLengthStateState::doRead() expectedNextBulkLength: {}", connection->m_context.expectedNextBulkLength);

    buffer->consume(bytesConsumed);

    connection->changeState(std::make_shared<ReadingArgumentState>(connection));

    if (bytesTransferred > bytesConsumed) {
        connection->m_state->doRead(buffer, bytesTransferred - bytesConsumed);
        return;
    }

    connection->doRead();
}

void ReadingArgumentState::doRead(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred)
{
    auto connection = m_connection.lock();
    if (!connection) {
        throw Exception("Failed to acquire Connection: weak_ptr expired");
    }

    buffer->commit(bytesTransferred);

    std::string_view data(static_cast<const char*>(buffer->data().data()), bytesTransferred);

    size_t pos = data.find("\r\n");
    if (pos == std::string::npos) {
        connection->doRead();
        return;
    }

    size_t bytesConsumed = pos + 2;

    std::string bulk(data.data(), pos);
    m_logger->debug("ReadingArgumentState::doRead() bulk: {}", bulk);

    if (bulk.length() != connection->m_context.expectedNextBulkLength) {
        m_logger->error("Failed to ReadingArgumentState::doRead(): length of '{}' ({}) does not match expected length ({})", bulk, bulk.size(), connection->m_context.expectedNextBulkLength);
        connection->changeState(std::make_shared<ErrorState>(connection, fmt::format("-ERR Protocol error: length of '{}' ({}) does not match expected length ({})", bulk, bulk.size(), connection->m_context.expectedNextBulkLength)));
        connection->m_state->doWrite(connection->m_writeBuffer, 0);
        return;
    }

    // First bulk is a name of command
    if (connection->m_context.cmd.length() == 0) {
        connection->m_context.cmd = stringToLower(bulk);
    }
    else {
        connection->m_context.args.push_back(bulk);
    }

    buffer->consume(bytesConsumed);

    connection->changeState(std::make_shared<ReadingArgumentLengthState>(connection));

    if (bytesTransferred > bytesConsumed) {
        connection->m_state->doRead(buffer, bytesTransferred - bytesConsumed);
        return;
    }

    if (connection->m_context.args.size() == connection->m_context.expectedArgsCount - 1) {
        auto connectionManager = connection->getConnectionManager().lock();
        auto server = connectionManager->getServer().lock();
        auto executor = server->getCommandExecutor().lock();

        CommandResult result = executor->execute(connection->m_context.cmd, connection->m_context.args);
        if (result.ok) {
            connection->changeState(std::make_shared<WritingResponseState>(connection, result.output));
            connection->m_state->doWrite(connection->m_writeBuffer, 0);
        }
        else {
            connection->changeState(std::make_shared<ErrorState>(connection, fmt::format("-ERR {}", result.output)));
            connection->m_state->doWrite(connection->m_writeBuffer, 0);
        }
    }
    else {
        connection->doRead();
    }
}

ErrorState::ErrorState(std::weak_ptr<Connection> connection, const std::string &output)
    : IConnectionState(connection)
    , m_output(output)
{}

void ErrorState::doWrite(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred)
{
    m_logger->debug("ErrorState::doWrite() bytesTransferred: {}", bytesTransferred);

    m_logger->debug("ErrorState::doWrite(), output: {}", m_output);

    auto connection = m_connection.lock();
    if (!connection) {
        throw Exception("Failed to acquire Connection: weak_ptr expired");
    }

    if (bytesTransferred > 0) {
        m_output.erase(0, bytesTransferred);
    }

    if (m_output.size() == 0) {
        connection->changeState(std::make_shared<ClosedState>(connection));
        connection->close();
        return;
    }

    auto outputBuffer = buffer->prepare(m_output.size());

    std::copy(m_output.begin(), m_output.end(), boost::asio::buffer_cast<unsigned char*>(outputBuffer));

    buffer->commit(m_output.size());

    connection->doWrite();
}

WritingResponseState::WritingResponseState(std::weak_ptr<Connection> connection, const std::string &output)
    : IConnectionState(connection)
    , m_output(output)
{}

void WritingResponseState::doWrite(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred)
{
    m_logger->debug("WritingResponseState::doWrite() bytesTransferred: {}", bytesTransferred);

    auto connection = m_connection.lock();
    if (!connection) {
        throw Exception("Failed to acquire Connection: weak_ptr expired");
    }

    if (bytesTransferred > 0) {
        m_output.erase(0, bytesTransferred);
    }

    if (m_output.size() == 0) {
        connection->changeState(std::make_shared<ClosedState>(connection));
        connection->close();
        return;
    }

    auto outputBuffer = buffer->prepare(m_output.size());

    std::copy(m_output.begin(), m_output.end(), boost::asio::buffer_cast<unsigned char*>(outputBuffer));

    buffer->commit(m_output.size());

    connection->doWrite();
}

} // namespace trogondb
