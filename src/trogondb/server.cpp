#include "trogondb/server.h"

#include <chrono>
#include <thread>
#include <vector>

#include "trogondb/os/process.h"
#include "trogondb/log/file_handler.h"
#include "trogondb/log/stream_handler.h"
#include "trogondb/log/rotating_file_handler.h"

namespace trogondb {

Server::Server(std::shared_ptr<Proactor> proactor, const std::shared_ptr<Config> &config)
    : Server(proactor, std::make_shared<Config>(*config))
{}

Server::Server(std::shared_ptr<Proactor> proactor, std::shared_ptr<Config> &&config)
    : m_config(std::move(config))
    , m_logger(createLogger(m_config))
    , m_proactor(proactor)
    , m_connectionManager()
    , m_acceptor()
    , m_store(std::make_shared<KeyValueStore>())
    , m_commandExecutor(std::make_shared<CommandExecutor>(m_store))
    , m_isInitialized(false)
{}

std::shared_ptr<log::Logger> Server::createLogger(const std::shared_ptr<Config> &config)
{
    std::vector<std::shared_ptr<log::Handler>> handlers;
    log::Level minLevel = log::Level::OFF;

    for (const auto &log : config->logs) {
        std::shared_ptr<log::Handler> handler;

        if (log.target == "stdout") {
            handler = std::make_shared<log::StreamHandler>(stdout);
        }
        else if (log.target == "stderr") {
            handler = std::make_shared<log::StreamHandler>(stderr);
        }
        else if (log.limit != 0) {
            handler = std::make_shared<log::RotatingFileHandler>(log.target, log.limit, log.rotate);
        }
        else {
            handler = std::make_shared<log::FileHandler>(log.target);
        }

        log::Level handlerLevel = log::getLevelByName(log.level);
        handler->setLevel(handlerLevel);

        if (minLevel > handlerLevel) {
            minLevel = handlerLevel;
        }

        handlers.push_back(handler);
    }

    auto logger = std::make_shared<log::Logger>("", handlers);
    logger->setLevel(minLevel);

    return logger;
}

void Server::initializeProcess(const std::shared_ptr<Config> &config)
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

void Server::initialize()
{
    m_connectionManager = std::make_shared<ConnectionManager>(shared_from_this());
    m_acceptor = std::make_shared<Acceptor>(m_proactor, m_connectionManager);

    m_acceptor->addListener(m_config->port);
    m_acceptor->run();
    m_acceptor->setNonBlocking(true);

    initializeProcess(m_config);
}

std::shared_ptr<log::Logger> Server::getLogger() const
{
    return m_logger;
}

std::shared_ptr<CommandExecutor> Server::getCommandExecutor() const
{
    return m_commandExecutor;
}

void Server::start()
{
    if (!m_isInitialized) {
        initialize();
        m_isInitialized = true;
    }

    m_logger->info("Starting server with pid {}", os::Process::getPid());
}

void Server::stop()
{
    // TODO
    m_acceptor->stop();
}

void Server::restart()
{
    // TODO
}

} // namespace trogondb
