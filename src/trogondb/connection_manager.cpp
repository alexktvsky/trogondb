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
    std::lock_guard<std::mutex> lock(m_mutex);
    auto connection = std::shared_ptr<Connection>(new Connection(shared_from_this(), std::move(socket)));
    m_connections.insert(connection);
    return connection;
}

void ConnectionManager::removeConnection(const std::shared_ptr<Connection> &connection)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    auto iter = m_connections.find(connection);
    if (iter == m_connections.end()) {
        return;
    }

    if (!connection->isClosed()) {
        connection->close();
    }

    m_connections.erase(iter);
}

void ConnectionManager::removeAll()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto &connection : m_connections) {
        if (connection) {
            connection->close();
        }
    }

    m_connections.clear();
}

} // namespace trogondb
