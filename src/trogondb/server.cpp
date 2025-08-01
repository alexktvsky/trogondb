#include "trogondb/server.h"

#include <chrono>
#include <thread>
#include <vector>

#include "trogondb/os/process.h"
#include "trogondb/log/log_manager.h"

namespace trogondb {

Server::Server(std::shared_ptr<Proactor> proactor, const std::shared_ptr<Config> &config)
    : Server(proactor, std::make_shared<Config>(*config))
{}

Server::Server(std::shared_ptr<Proactor> proactor, std::shared_ptr<Config> &&config)
    : m_config(std::move(config))
    , m_logger(log::LogManager::instance().getDefaultLogger())
    , m_proactor(proactor)
    , m_connectionManager()
    , m_acceptor()
    , m_store(std::make_shared<KeyValueStore>())
    , m_commandExecutor(std::make_shared<CommandExecutor>(m_store))
    , m_initialized(false)
    , m_running(false)
{}

void Server::initialize()
{
    if (m_initialized) {
        return;
    }
    m_initialized = true;

    m_connectionManager = std::make_shared<ConnectionManager>(shared_from_this());
    m_acceptor = std::make_shared<Acceptor>(m_proactor, m_connectionManager);

    m_acceptor->addListener(m_config->port);
    m_acceptor->setNonBlocking(true);
}

std::shared_ptr<log::Logger> Server::getLogger() const
{
    return m_logger;
}

std::weak_ptr<CommandExecutor> Server::getCommandExecutor() const
{
    return m_commandExecutor;
}

void Server::start()
{
    if (m_running) {
        return;
    }

    m_running = true;
    m_logger->info("Starting server with pid {}", os::Process::getPid());

    initialize();
    m_acceptor->run();
}

void Server::stop()
{
    if (!m_running) {
        return;
    }

    m_logger->info("Stopping server...");

    if (m_acceptor) {
        m_acceptor->stop();
    }

    m_connectionManager->closeAll();

    m_logger->info("Server stopped");
}

void Server::restart()
{
    // TODO
}

bool Server::isRunning() const
{
    return m_running;
}

} // namespace trogondb
