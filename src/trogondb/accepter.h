#pragma once

#include <boost/asio.hpp>

namespace trogondb {

class Accepter {
public:

private:
    boost::asio::ip::tcp::acceptor m_acceptor;
};

} // namespace trogondb
