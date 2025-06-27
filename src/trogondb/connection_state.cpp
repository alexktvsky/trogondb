#include "trogondb/connection_state.h"

namespace trogondb {

IConnectionState::IConnectionState(std::shared_ptr<Connection> connection)
    : m_connection(connection)
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



    // buffer->consume(bytesTransferred);
}

} // namespace trogondb
