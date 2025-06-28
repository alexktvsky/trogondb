#include "trogondb/connection_state.h"

#include <string>

#include "trogondb/log/log_manager.h"

namespace trogondb {

IConnectionState::IConnectionState(std::shared_ptr<Connection> connection)
    : m_connection(connection)
    , m_logger(log::LogManager::instance().getDefaultLogger())
{}

void IConnectionState::doRead(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred)
{}

void IConnectionState::doWrite()
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
        return;
    }

    size_t bytesConsumed = pos + 2;

    if (data[0] != '*') {
        m_logger->error("Failed ReadingHeaderState::doRead(): expected '*'");
        m_connection->changeState(std::make_shared<ErrorState>(m_connection));
        m_connection->cancel();
        return;
    }

    m_connection->m_context.expectedArgsCount = std::stoi(data.substr(1, pos)); // skip '*'
    m_logger->debug("ReadingHeaderState::doRead() m_expectedArgsCount: {}", m_connection->m_context.expectedArgsCount);

    buffer->consume(bytesConsumed);

    m_connection->changeState(std::make_shared<ReadingArgumentLengthState>(m_connection));

    if (bytesTransferred > bytesConsumed) {
        m_connection->m_state->doRead(buffer, bytesTransferred - bytesConsumed);
    }
}

void ReadingArgumentLengthState::doRead(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred)
{
    buffer->commit(bytesTransferred);

    std::string data(boost::asio::buffers_begin(buffer->data()),
                     boost::asio::buffers_begin(buffer->data()) + bytesTransferred);

    size_t pos = data.find("\r\n");
    if (pos == std::string::npos) {
        return;
    }

    size_t bytesConsumed = pos + 2;

    if (data[0] != '$') {
        m_logger->error("Failed ReadingArgumentLengthState::doRead(): expected '$'");
        m_connection->changeState(std::make_shared<ErrorState>(m_connection));
        m_connection->cancel();
        return;
    }

    m_connection->m_context.expectedNextBulkLength = std::stoi(data.substr(1, pos)); // skip '$'
    m_logger->debug("ArgumentLengthStateState::doRead() expectedNextBulkLength: {}", m_connection->m_context.expectedNextBulkLength);

    buffer->consume(bytesConsumed);

    m_connection->changeState(std::make_shared<ReadingArgumentState>(m_connection));

    if (bytesTransferred > bytesConsumed) {
        m_connection->m_state->doRead(buffer, bytesTransferred - bytesConsumed);
    }
}

void ReadingArgumentState::doRead(std::shared_ptr<boost::asio::streambuf> buffer, size_t bytesTransferred)
{
    buffer->commit(bytesTransferred);

    std::string data(boost::asio::buffers_begin(buffer->data()),
                     boost::asio::buffers_begin(buffer->data()) + bytesTransferred);

    size_t pos = data.find("\r\n");
    if (pos == std::string::npos) {
        return;
    }

    size_t bytesConsumed = pos + 2;

    std::string bulk = data.substr(0, pos);
    m_logger->debug("ReadingArgumentState::doRead() bulk: {}", bulk);

    if (bulk.length() != m_connection->m_context.expectedNextBulkLength) {
        m_logger->error("Failed to ReadingArgumentState::doRead(): length of '{}' ({}) does not match expected length ({})", bulk, bulk.size(), m_connection->m_context.expectedNextBulkLength);
    }

    // First bulk is a name of command
    if (m_connection->m_context.cmd.length() == 0) {
        m_connection->m_context.cmd = bulk;
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
        m_logger->debug("execute command {}", m_connection->m_context.cmd);
        // execute command
        // m_connection->changeState(std::make_shared<XXX>(m_connection));
    }
}

} // namespace trogondb
