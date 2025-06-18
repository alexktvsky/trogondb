#include "trogondb/server.h"

#include <chrono>
#include <thread>

#include "trogondb/os/process.h"
#include "trogondb/log/file_handler.h"
#include "trogondb/log/stream_handler.h"
#include "trogondb/log/rotating_file_handler.h"

namespace trogondb {

Server::Server(std::shared_ptr<Proactor> proactor, const std::shared_ptr<Config> &config)
    : Server(proactor, std::make_shared<Config>(*config)) {}

Server::Server(std::shared_ptr<Proactor> proactor, std::shared_ptr<Config> &&config)
    : m_config(std::move(config))
    , m_logger(createLogger(m_config))
    , m_proactor(std::make_shared<Proactor>())
    , m_sessions()
    , m_store(std::make_shared<KeyValueStore>())
{}

std::shared_ptr<log::Logger> Server::createLogger(const std::shared_ptr<Config> &config)
{
    std::list<std::shared_ptr<log::Handler>> handlers;
    std::shared_ptr<log::Handler> handler;

    for (const auto &log : config->logs) {
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

        handler->setLevel(log::getLevelByName(log.level));
        handlers.push_back(handler);
    }

    return std::make_shared<log::Logger>(handlers);
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
    initializeProcess(m_config);

    // m_acceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(*m_io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), m_config->port));
}

void Server::start()
{
    initialize();
    m_logger->info("Starting server with pid {}", os::Process::getPid());

    m_proactor->run();
}

void Server::stop()
{
    // TODO
}

void Server::restart()
{
    // TODO
}

// void Server::doAccept()
// {
//     m_acceptor->async_accept(std::bind(&Server::onAccept, this, std::placeholders::_1, std::placeholders::_2));
// }

// void Server::onAccept(const boost::system::error_code &err, boost::asio::ip::tcp::socket socket)
// {
//     if (!err) {
//         auto session = createSession(std::move(socket));
//         session->start();
//     }
//     else {
//         m_logger->error("Failed to onAccept(): {}", err.message());
//     }

//     doAccept();
// }

// std::shared_ptr<Session> Server::createSession(boost::asio::ip::tcp::socket socket)
// {
//     auto session = std::make_shared<Session>(std::move(socket), m_store, m_logger);
//     m_sessions.push_back(session);
//     return session;
// }

void Server::removeSession(const std::shared_ptr<Session> &session)
{
    session->cancel();
    m_sessions.remove(session);
}

} // namespace trogondb
