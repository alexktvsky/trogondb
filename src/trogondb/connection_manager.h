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
    friend class Connection;

public:
    ConnectionManager(std::weak_ptr<Server> server);

    std::weak_ptr<Server> getServer() const;

    std::shared_ptr<Connection> create(boost::asio::ip::tcp::socket socket);

    void closeAll();

private:
    void remove(const std::shared_ptr<Connection> &connection);

    std::weak_ptr<Server> m_server;
    std::unordered_set<std::shared_ptr<Connection>> m_connections;
    std::mutex m_mutex;
};

} // namespace trogondb
