#include "trogondb/connection.h"

#include <fmt/format.h>

#include "trogondb/log/log_manager.h"

namespace trogondb {

Connection::Connection(boost::asio::ip::tcp::socket socket)
    : m_socket(std::move(socket))
    , m_logger(log::LogManager::instance().getDefaultLogger())
{}

void Connection::start()
{
    m_logger->info("Started new connection {}", m_socket.remote_endpoint().address().to_string());

}

void Connection::cancel()
{
}

} // namespace trogondb
