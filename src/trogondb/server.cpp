#include "server.h"

#include <chrono>
#include <thread>

#include "trogondb/logging/file_handler.h"
#include "trogondb/logging/stream_handler.h"
#include "trogondb/logging/rotating_file_handler.h"

namespace trogondb {

Server::Server(const std::shared_ptr<ServerConfig> &config)
    : Server(std::make_shared<ServerConfig>(*config)) {}

Server::Server(std::shared_ptr<ServerConfig> &&config)
    : m_config(std::move(config))
    , m_logger(configureLogger(m_config))
{
    // ...
}

void Server::configureProcess(const std::shared_ptr<ServerConfig> &config)
{
    if (config->daemon) {
        os::Process::becomeDaemon();
    }

    if (!config->workdir.empty()) {
        os::Process::setWorkingDirectory(config->workdir);
    }

    if (config->priority != 0) {
        os::Process::setPriority(config->priority);
    }

    if (!config->user.empty()) {
        os::Process::setUser(config->user);
    }

    if (!config->group.empty()) {
        os::Process::setGroup(config->group);
    }
}

std::shared_ptr<logging::Logger> Server::configureLogger(const std::shared_ptr<ServerConfig> &config)
{
    std::list<std::shared_ptr<logging::Handler>> handlers;
    std::shared_ptr<logging::Handler> handler;

    for (const auto &log : config->logs) {
        if (log.target == "stdout") {
            handler = std::make_shared<logging::StreamHandler>(stdout);
        }
        else if (log.target == "stderr") {
            handler = std::make_shared<logging::StreamHandler>(stderr);
        }
        else if (log.limit != 0) {
            handler = std::make_shared<logging::RotatingFileHandler>(log.target, log.limit, log.rotate);
        }
        else {
            handler = std::make_shared<logging::FileHandler>(log.target);
        }

        handler->setLevel(logging::getLevelByName(log.level));
        handlers.push_back(handler);
    }

    return std::make_shared<logging::Logger>(handlers);
}

void Server::initialize()
{
    configureProcess(m_config);
}

void Server::start()
{
    initialize();

    m_logger->info("Starting server with pid {} at dir {}", os::Process::getPid(), os::Process::getWorkingDirectory());

    std::this_thread::sleep_for(std::chrono::minutes(3));
}

void Server::stop()
{
    // ...
}

void Server::restart()
{
    // ...
}

} // namespace trogondb
