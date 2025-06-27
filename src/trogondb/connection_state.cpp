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

    size_t elementsCount = std::stoi(data.substr(1, pos));

    m_logger->debug("ReadingHeaderState::doRead() elementsCount: {}", elementsCount);

    buffer->consume(bytesConsumed);

    m_connection->changeState(std::make_shared<ReadingArgumentLengthState>(m_connection));

    if (bytesTransferred > bytesConsumed) {
        m_connection->m_state->doRead(buffer, bytesTransferred - bytesConsumed);
    }
}


} // namespace trogondb
