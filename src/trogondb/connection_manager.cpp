#include "trogondb/connection_manager.h"

namespace trogondb {

ConnectionManager::ConnectionManager(std::weak_ptr<Server> server)
    : m_server(server)
    , m_connections()
{}

std::weak_ptr<Server> ConnectionManager::getServer() const
{
    return m_server;
}

std::shared_ptr<Connection> ConnectionManager::createConnection(boost::asio::ip::tcp::socket socket)
{
    auto connection = std::make_shared<Connection>(shared_from_this(), std::move(socket));
    m_connections.push_back(connection);
    return connection;
}

void ConnectionManager::removeConnection(const std::shared_ptr<Connection> &connection)
{
    if (!connection->isClosed()) {
        connection->close();
    }
    m_connections.remove(connection);
}

} // namespace trogondb
