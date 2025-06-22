#pragma once

#include <memory>
#include <list>

#include "trogondb/connection.h"

namespace trogondb {

class ConnectionManager {
public:
    std::shared_ptr<Connection> createConnection(boost::asio::ip::tcp::socket socket);
    void removeConnection(const std::shared_ptr<Connection> &connection);
private:
    std::list<std::shared_ptr<Connection>> m_connections;
};

} // namespace trogondb
