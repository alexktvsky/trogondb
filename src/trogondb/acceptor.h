#pragma once

#include <cstdint>
#include <memory>
#include <boost/asio.hpp>

#include "trogondb/proactor.h"

namespace trogondb {

class Acceptor {
public:
    explicit Acceptor(std::shared_ptr<Proactor> proactor);

    void addListener(uint16_t port);

    void setNonBlocking(bool mode);

    void run();

private:
    std::shared_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;
};

} // namespace trogondb

