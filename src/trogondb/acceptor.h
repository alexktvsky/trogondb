#pragma once

#include <cstdint>
#include <memory>
#include <atomic>
#include <mutex>
#include <boost/asio.hpp>

#include "trogondb/proactor.h"
#include "trogondb/connection_manager.h"
#include "trogondb/log/logger.h"

namespace trogondb {

class ConnectionManager;

class Acceptor : public std::enable_shared_from_this<Acceptor> {
public:
    explicit Acceptor(std::weak_ptr<Proactor> proactor, std::weak_ptr<ConnectionManager> connectionManager);

    void addListener(uint16_t port);

    void setNonBlocking(bool mode);

    void run();

    void stop();

    bool isStopped() const;

private:
    void accept();

    void onAccept(const boost::system::error_code &err, boost::asio::ip::tcp::socket socket);

    std::shared_ptr<log::Logger> m_logger;
    std::shared_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;
    std::weak_ptr<ConnectionManager> m_connectionManager;
    std::atomic<bool> m_stopped;
    std::mutex m_stopMutex;
};

} // namespace trogondb

