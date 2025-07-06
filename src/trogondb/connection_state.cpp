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
    buffer->commit(bytesTransferred);

    std::string data(boost::asio::buffers_begin(buffer->data()),
                     boost::asio::buffers_begin(buffer->data()) + bytesTransferred);

    size_t pos = data.find("\r\n");
    if (pos == std::string::npos) {
        m_connection.lock()->doRead();
        return;
    }

    size_t bytesConsumed = pos + 2;

    if (data[0] != '*') {
        m_logger->error("Failed ReadingHeaderState::doRead(): expected '*', got '{}'", data[0]);
        m_connection.lock()->changeState(std::make_shared<ErrorState>(m_connection.lock(), fmt::format("-ERR Protocol error: expected '*', got '{}'", data[0])));
        m_connection.lock()->m_state->doWrite(m_connection.lock()->m_writeBuffer, 0);
        return;
    }

    std::string expectedArgsStr = data.substr(1, pos); // skip '*'
    auto expectedArgsCount = stringToNumber<uint32_t>(expectedArgsStr);
    if (!expectedArgsCount) {
        m_logger->error("Failed to ReadingHeaderState::doRead(): invalid multibulk length");
        m_connection.lock()->changeState(std::make_shared<ErrorState>(m_connection.lock(), "-ERR Protocol error: invalid multibulk length"));
        m_connection.lock()->m_state->doWrite(m_connection.lock()->m_writeBuffer, 0);
        return;
    }

    m_connection.lock()->m_context.expectedArgsCount = expectedArgsCount.value();
    m_logger->debug("ReadingHeaderState::doRead() expectedArgsCount: {}", m_connection.lock()->m_context.expectedArgsCount);

    buffer->consume(bytesConsumed);

    m_connection.lock()->changeState(std::make_shared<ReadingArgumentLengthState>(m_connection.lock()));

    if (bytesTransferred > bytesConsumed) {
        m_connection.lock()->m_state->doRead(buffer, bytesTransferred - bytesConsumed);
        return;
    }

    m_connection.lock()->doRead();
}

void ReadingArgumentLengthState::doRead(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred)
{
    buffer->commit(bytesTransferred);

    std::string data(boost::asio::buffers_begin(buffer->data()),
                     boost::asio::buffers_begin(buffer->data()) + bytesTransferred);

    size_t pos = data.find("\r\n");
    if (pos == std::string::npos) {
        m_connection.lock()->doRead();
        return;
    }

    size_t bytesConsumed = pos + 2;

    if (data[0] != '$') {
        m_logger->error("Failed ReadingArgumentLengthState::doRead(): expected '$', got '{}'", data[0]);
        m_connection.lock()->changeState(std::make_shared<ErrorState>(m_connection.lock(), fmt::format("-ERR Protocol error: expected '$', got '{}'", data[0])));
        m_connection.lock()->m_state->doWrite(m_connection.lock()->m_writeBuffer, 0);
        return;
    }

    std::string bulkLengthStr = data.substr(1, pos); // skip '$'
    auto bulkLength = stringToNumber<uint32_t>(bulkLengthStr);
    if (!bulkLength) {
        m_logger->error("Failed to ReadingArgumentLengthState::doRead(): invalid bulk length");
        m_connection.lock()->changeState(std::make_shared<ErrorState>(m_connection.lock(), "-ERR Protocol error: invalid bulk length"));
        m_connection.lock()->m_state->doWrite(m_connection.lock()->m_writeBuffer, 0);
        return;
    }

    m_connection.lock()->m_context.expectedNextBulkLength = bulkLength.value();
    m_logger->debug("ArgumentLengthStateState::doRead() expectedNextBulkLength: {}", m_connection.lock()->m_context.expectedNextBulkLength);

    buffer->consume(bytesConsumed);

    m_connection.lock()->changeState(std::make_shared<ReadingArgumentState>(m_connection.lock()));

    if (bytesTransferred > bytesConsumed) {
        m_connection.lock()->m_state->doRead(buffer, bytesTransferred - bytesConsumed);
        return;
    }

    m_connection.lock()->doRead();
}

void ReadingArgumentState::doRead(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred)
{
    buffer->commit(bytesTransferred);

    std::string data(boost::asio::buffers_begin(buffer->data()),
                     boost::asio::buffers_begin(buffer->data()) + bytesTransferred);

    size_t pos = data.find("\r\n");
    if (pos == std::string::npos) {
        m_connection.lock()->doRead();
        return;
    }

    size_t bytesConsumed = pos + 2;

    std::string bulk = data.substr(0, pos);
    m_logger->debug("ReadingArgumentState::doRead() bulk: {}", bulk);

    if (bulk.length() != m_connection.lock()->m_context.expectedNextBulkLength) {
        m_logger->error("Failed to ReadingArgumentState::doRead(): length of '{}' ({}) does not match expected length ({})", bulk, bulk.size(), m_connection.lock()->m_context.expectedNextBulkLength);
        m_connection.lock()->changeState(std::make_shared<ErrorState>(m_connection.lock(), fmt::format("-ERR Protocol error: length of '{}' ({}) does not match expected length ({})", bulk, bulk.size(), m_connection.lock()->m_context.expectedNextBulkLength)));
        m_connection.lock()->m_state->doWrite(m_connection.lock()->m_writeBuffer, 0);
        return;
    }

    // First bulk is a name of command
    if (m_connection.lock()->m_context.cmd.length() == 0) {
        m_connection.lock()->m_context.cmd = stringToLower(bulk);
    }
    else {
        m_connection.lock()->m_context.args.push_back(bulk);
    }

    buffer->consume(bytesConsumed);

    m_connection.lock()->changeState(std::make_shared<ReadingArgumentLengthState>(m_connection.lock()));

    if (bytesTransferred > bytesConsumed) {
        m_connection.lock()->m_state->doRead(buffer, bytesTransferred - bytesConsumed);
        return;
    }

    if (m_connection.lock()->m_context.args.size() == m_connection.lock()->m_context.expectedArgsCount - 1) {
        auto executor = m_connection.lock()->getConnectionManager().lock()->getServer().lock()->getCommandExecutor().lock();
        m_logger->debug("Executing command '{}' with {} args", m_connection.lock()->m_context.cmd, m_connection.lock()->m_context.args.size());

        CommandResult result = executor->execute(m_connection.lock()->m_context.cmd, m_connection.lock()->m_context.args);
        if (result.ok) {
            m_connection.lock()->changeState(std::make_shared<WritingResponseState>(m_connection.lock(), result.output));
            m_connection.lock()->m_state->doWrite(m_connection.lock()->m_writeBuffer, 0);
        }
        else {
            m_connection.lock()->changeState(std::make_shared<ErrorState>(m_connection.lock(), fmt::format("-ERR {}", result.output)));
            m_connection.lock()->m_state->doWrite(m_connection.lock()->m_writeBuffer, 0);
        }
    }
    else {
        m_connection.lock()->doRead();
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

    if (bytesTransferred > 0) {
        m_output.erase(0, bytesTransferred);
    }

    if (m_output.size() == 0) {
        m_connection.lock()->close();
        m_connection.lock()->changeState(std::make_shared<ClosedState>(m_connection.lock()));
        m_connection.lock()->getConnectionManager().lock()->removeConnection(m_connection.lock());
        return;
    }

    auto outputBuffer = buffer->prepare(m_output.size());

    std::copy(m_output.begin(), m_output.end(), boost::asio::buffer_cast<unsigned char*>(outputBuffer));

    buffer->commit(m_output.size());

    m_connection.lock()->doWrite();
}

WritingResponseState::WritingResponseState(std::weak_ptr<Connection> connection, const std::string &output)
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
        m_connection.lock()->close();
        m_connection.lock()->changeState(std::make_shared<ClosedState>(m_connection.lock()));
        m_connection.lock()->getConnectionManager().lock()->removeConnection(m_connection.lock());
        return;
    }

    auto outputBuffer = buffer->prepare(m_output.size());

    std::copy(m_output.begin(), m_output.end(), boost::asio::buffer_cast<unsigned char*>(outputBuffer));

    buffer->commit(m_output.size());

    m_connection.lock()->doWrite();
}

} // namespace trogondb
