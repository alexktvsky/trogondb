#pragma once

#include <memory>
#include <list>

#include "trogondb/config.h"
#include "trogondb/log/logger.h"
#include "trogondb/kv_store.h"
#include "trogondb/proactor.h"
#include "trogondb/acceptor.h"
#include "trogondb/connection.h"
#include "trogondb/connection_manager.h"

namespace trogondb {

class Server {
public:
    Server(std::shared_ptr<Proactor> proactor, const std::shared_ptr<Config> &config);
    Server(std::shared_ptr<Proactor> proactor, std::shared_ptr<Config> &&config);
    std::shared_ptr<log::Logger> getLogger() const;
    void start();
    void stop();
    void restart();

private:
    static std::shared_ptr<log::Logger> createLogger(const std::shared_ptr<Config> &config);
    void initializeProcess(const std::shared_ptr<Config> &config);
    void initialize();

    std::shared_ptr<Config> m_config;
    std::shared_ptr<log::Logger> m_logger;
    std::shared_ptr<Proactor> m_proactor;
    std::shared_ptr<ConnectionManager> m_connectionManager;
    std::shared_ptr<Acceptor> m_accepter;

    std::shared_ptr<KeyValueStore> m_store;
};

} // namespace trogondb
