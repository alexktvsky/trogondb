#pragma once

#include <cstdint>
#include <memory>
#include <atomic>
#include <functional>
#include <boost/asio.hpp>

#include "trogondb/proactor.h"

namespace trogondb {

class Acceptor {
public:
    explicit Acceptor(std::shared_ptr<Proactor> proactor);

    void addListener(uint16_t port);

    void setNonBlocking(bool mode);

    void run();

    void stop();

private:
    void accept();

    void onAccept(const boost::system::error_code &err, boost::asio::ip::tcp::socket socket);

    std::shared_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;

    std::atomic<bool> m_stopped;
};

} // namespace trogondb

