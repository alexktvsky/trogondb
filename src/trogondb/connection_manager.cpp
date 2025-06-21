#include "trogondb/connection_manager.h"

namespace trogondb {

ConnectionManager::ConnectionManager()
{

}

std::shared_ptr<Connection> ConnectionManager::createConnection(boost::asio::ip::tcp::socket socket)
{
    auto connection = std::make_shared<Connection>(std::move(socket), m_logger);
    m_connections.push_back(connection);
    return connection;
}

void ConnectionManager::removeConnection(const std::shared_ptr<Connection> &connection)
{
    connection->cancel();
    m_connections.remove(connection);
}

} // namespace trogondb
