#include "trogondb/server.h"

#include <chrono>
#include <thread>

#include "trogondb/logging/file_handler.h"
#include "trogondb/logging/stream_handler.h"
#include "trogondb/logging/rotating_file_handler.h"

namespace trogondb {

Server::Server(const std::shared_ptr<Config> &config)
    : Server(std::make_shared<Config>(*config)) {}

Server::Server(std::shared_ptr<Config> &&config)
    : m_config(std::move(config))
    , m_logger(createLogger(m_config))
    , m_store()
    , m_io(std::make_shared<boost::asio::io_context>())
{
    // ...
}

std::shared_ptr<logging::Logger> Server::createLogger(const std::shared_ptr<Config> &config)
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

    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), m_config->port);
    m_acceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(*m_io, endpoint);
}

void Server::start()
{
    initialize();

    m_logger->info("Starting server with pid {} at dir {}", os::Process::getPid(), os::Process::getWorkingDirectory());

    doAccept();

    m_io->run();
}

void Server::stop()
{
    // ...
}

void Server::restart()
{
    // ...
}

void Server::doAccept()
{
    m_acceptor->async_accept(std::bind(&Server::onAccept, this, std::placeholders::_1, std::placeholders::_2));
}

void Server::onAccept(const boost::system::error_code &err, boost::asio::ip::tcp::socket socket)
{
    if (!err) {
        auto session = std::make_shared<Session>(std::move(socket), m_store);
        m_sessions.push_back(session);
        session->start();
    }
    std::printf("****\n");
    doAccept();
}

void Server::removeSession(const std::shared_ptr<Session> &session)
{
    m_sessions.remove(session);
}

} // namespace trogondb
