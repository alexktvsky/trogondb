#pragma once

#include <memory>

#include "trogondb/config.h"
#include "trogondb/log/logger.h"
#include "trogondb/kv_store.h"
#include "trogondb/proactor.h"
#include "trogondb/acceptor.h"
#include "trogondb/connection.h"
#include "trogondb/connection_manager.h"
#include "trogondb/command_executor.h"

namespace trogondb {

class Server : public std::enable_shared_from_this<Server> {
public:
    Server(std::shared_ptr<Proactor> proactor, const std::shared_ptr<Config> &config);

    Server(std::shared_ptr<Proactor> proactor, std::shared_ptr<Config> &&config);

    std::shared_ptr<log::Logger> getLogger() const;

    std::weak_ptr<CommandExecutor> getCommandExecutor() const;

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
    std::shared_ptr<Acceptor> m_acceptor;
    std::shared_ptr<KeyValueStore> m_store;
    std::shared_ptr<CommandExecutor> m_commandExecutor;

    std::atomic<bool> m_initialized;
    std::mutex m_initMutex;
    std::atomic<bool> m_stopped;
    std::mutex m_stopMutex;
};

} // namespace trogondb
