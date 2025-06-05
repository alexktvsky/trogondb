#pragma once

#include <memory>
#include <boost/asio.hpp>

#include "trogondb/config.h"
#include "trogondb/os/process.h"
#include "trogondb/logging/logger.h"
#include "trogondb/store.h"
#include "trogondb/session.h"

namespace trogondb {

class Server {
public:
    Server(const std::shared_ptr<Config> &config);
    Server(std::shared_ptr<Config> &&config);
    void start();
    void stop();
    void restart();

private:
    static std::shared_ptr<logging::Logger> createLogger(const std::shared_ptr<Config> &config);
    void initializeProcess(const std::shared_ptr<Config> &config);
    void initialize();

    void doAccept();
    void onAccept(const boost::system::error_code &err, boost::asio::ip::tcp::socket socket);
    void removeSession(const std::shared_ptr<Session> &session);

    std::shared_ptr<Config> m_config;
    std::shared_ptr<logging::Logger> m_logger;
    std::shared_ptr<Store> m_store;

    std::shared_ptr<boost::asio::io_context> m_io;
    std::shared_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;
    std::list<std::shared_ptr<Session>> m_sessions;
};

} // namespace trogondb
