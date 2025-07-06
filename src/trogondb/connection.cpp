#include "trogondb/connection.h"

#include <fmt/format.h>

#include "trogondb/log/log_manager.h"
#include "trogondb/exception.h"

namespace trogondb {

constexpr size_t READ_BUFFER_SIZE = 1024;

Connection::Connection(std::weak_ptr<ConnectionManager>connectionManager, boost::asio::ip::tcp::socket socket)
    : m_logger(log::LogManager::instance().getDefaultLogger())
    , m_connectionManager(connectionManager)
    , m_state(nullptr)
    , m_socket(std::move(socket))
    , m_readBuffer(std::make_shared<boost::asio::streambuf>())
    , m_writeBuffer(std::make_shared<boost::asio::streambuf>())
{}

void Connection::start()
{
    m_logger->info("Started new connection {}", m_socket.remote_endpoint().address().to_string());

    changeState(std::make_shared<ReadingHeaderState>(shared_from_this()));

    doRead();
}

void Connection::close()
{
    if (!m_socket.is_open()) {
        return;
    }

    m_logger->info("Closing connection {}", m_socket.remote_endpoint().address().to_string());

    m_socket.cancel();
}

bool Connection::isClosed() const
{
    return !m_socket.is_open();
}

std::weak_ptr<ConnectionManager> Connection::getConnectionManager() const
{
    return m_connectionManager;
}

void Connection::changeState(std::shared_ptr<IConnectionState> state)
{
    m_state = state;
}

void Connection::doRead()
{
    if (!m_socket.is_open()) {
        return;
    }

    auto buffer = m_readBuffer->prepare(READ_BUFFER_SIZE);

    m_socket.async_read_some(buffer, std::bind(&Connection::onReadDone, shared_from_this(), std::placeholders::_1, std::placeholders::_2));

    // startTimeout();
}

void Connection::onReadDone(const boost::system::error_code &err, size_t bytesTransferred)
{
    if (err) {
        if (err == boost::asio::error::eof) {
            m_logger->debug("Client closed connection");
        }
        else {
            m_logger->error("Failed to onReadDone(): {}", err.message());
        }

        close();
        return;
    }

    if (!m_state) {
        throw Exception("State is null in Connection::onReadDone()");
    }

    m_state->doRead(m_readBuffer, bytesTransferred);
}

void Connection::doWrite()
{
    m_socket.async_write_some(m_writeBuffer->data(), std::bind(&Connection::onWriteDone, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void Connection::onWriteDone(const boost::system::error_code &err, size_t bytesTransferred)
{
    m_state->doWrite(m_writeBuffer, bytesTransferred);
}

} // namespace trogondb
