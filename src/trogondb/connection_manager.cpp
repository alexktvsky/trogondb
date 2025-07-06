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

std::shared_ptr<Connection> ConnectionManager::create(boost::asio::ip::tcp::socket socket)
{
    auto connection = std::shared_ptr<Connection>(new Connection(std::move(socket), shared_from_this()));

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_connections.insert(connection);
    }

    return connection;
}

void ConnectionManager::remove(const std::shared_ptr<Connection> &connection)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_connections.erase(connection);
}

void ConnectionManager::closeAll()
{
    std::unordered_set<std::shared_ptr<Connection>> snapshot;

    {
        std::lock_guard lk(m_mutex);
        snapshot.swap(m_connections);
    }

    for (auto &connection : snapshot) {
        connection->close();
    }
}

} // namespace trogondb
