#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <memory>
#include <boost/asio.hpp>

#include "trogondb/log/logger.h"
#include "trogondb/connection_state.h"

namespace trogondb {

class Connection : public std::enable_shared_from_this<Connection> {
public:
    Connection(boost::asio::ip::tcp::socket socket);

    void start();

    void cancel();

private:
    boost::asio::ip::tcp::socket m_socket;
    std::shared_ptr<log::Logger> m_logger;

    boost::asio::streambuf m_readBuffer;
    boost::asio::streambuf m_writeBuffer;
    // boost::asio::steady_timer m_timer;

    std::shared_ptr<IConnectionState> m_state;
};

} // namespace trogondb
