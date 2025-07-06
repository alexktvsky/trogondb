#pragma once

#include <memory>
#include <unordered_set>
#include <mutex>

#include "trogondb/connection.h"
#include "trogondb/server.h"
#include "trogondb/log/logger.h"

namespace trogondb {

class Server;
class Connection;

class ConnectionManager : public std::enable_shared_from_this<ConnectionManager> {
public:
    ConnectionManager(std::weak_ptr<Server> server);

    std::weak_ptr<Server> getServer() const;

    std::shared_ptr<Connection> createConnection(boost::asio::ip::tcp::socket socket);

    void removeConnection(const std::shared_ptr<Connection> &connection);

    void closeAll();

private:
    std::weak_ptr<Server> m_server;
    std::unordered_set<std::shared_ptr<Connection>> m_connections;
    std::mutex m_mutex;
};

} // namespace trogondb
