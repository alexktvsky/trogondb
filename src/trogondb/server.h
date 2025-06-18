#pragma once

#include <list>
#include <memory>

#include "trogondb/config.h"
#include "trogondb/log/logger.h"
#include "trogondb/kv_store.h"
#include "trogondb/proactor.h"
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
    static std::shared_ptr<log::Logger> createLogger(const std::shared_ptr<Config> &config);
    void initializeProcess(const std::shared_ptr<Config> &config);
    void initialize();

    std::shared_ptr<Session> createSession(boost::asio::ip::tcp::socket socket);
    void removeSession(const std::shared_ptr<Session> &session);

private:
    std::shared_ptr<Config> m_config;
    std::shared_ptr<log::Logger> m_logger;

    std::shared_ptr<KeyValueStore> m_store;
    std::list<std::shared_ptr<Session>> m_sessions;

    std::shared_ptr<Proactor> m_proactor;



    // std::shared_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;
};

} // namespace trogondb
