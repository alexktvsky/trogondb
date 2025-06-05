#pragma once

#include <memory>
#include <boost/asio.hpp> // remove me

#include "trogondb/config.h"
#include "trogondb/os/process.h"
#include "trogondb/logging/logger.h"

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

    std::shared_ptr<Config> m_config;
    std::shared_ptr<logging::Logger> m_logger;
};

} // namespace trogondb
