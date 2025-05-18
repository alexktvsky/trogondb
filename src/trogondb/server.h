#pragma once

#include <memory>

#include "trogondb/server_config.h"
#include "trogondb/os/process.h"
#include "trogondb/logging/logger.h"

namespace trogondb {

class Server {
public:
    Server(const std::shared_ptr<ServerConfig> &config);
    Server(std::shared_ptr<ServerConfig> &&config);
    void start();
    void stop();
    void restart();
private:
    void initialize();
    void configureProcess(const std::shared_ptr<ServerConfig> &config);
    static std::shared_ptr<logging::Logger> configureLogger(const std::shared_ptr<ServerConfig> &config);
private:
    std::shared_ptr<ServerConfig> m_config;
    std::shared_ptr<logging::Logger> m_logger;
};

} // namespace trogondb
