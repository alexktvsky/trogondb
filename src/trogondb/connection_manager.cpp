#include "trogondb/connection_manager.h"

namespace trogondb {

std::shared_ptr<Connection> ConnectionManager::createConnection(
    boost::asio::ip::tcp::socket socket,
    std::shared_ptr<log::Logger> logger)
{
    auto connection = std::make_shared<Connection>(std::move(socket), logger);
    m_connections.push_back(connection);
    return connection;
}

void ConnectionManager::removeConnection(const std::shared_ptr<Connection> &connection)
{
    connection->cancel();
    m_connections.remove(connection);
}

} // namespace trogondb
