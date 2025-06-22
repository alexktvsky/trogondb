#include "trogondb/acceptor.h"

namespace trogondb {

Acceptor::Acceptor(std::shared_ptr<Proactor> proactor,
                   std::shared_ptr<ConnectionManager> connectionManager,
                   std::shared_ptr<log::Logger> logger)
    : m_acceptor(std::make_shared<boost::asio::ip::tcp::acceptor>(*proactor->getImpl()))
    , m_stopped(false)
    , m_connectionManager(connectionManager)
    , m_logger(logger)
{}

void Acceptor::setNonBlocking(bool mode)
{
    m_acceptor->non_blocking(mode);
}

void Acceptor::addListener(uint16_t port)
{
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
    m_acceptor->open(endpoint.protocol());
    m_acceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    m_acceptor->bind(endpoint);
}

void Acceptor::run()
{
    m_acceptor->listen();
    accept();
}

void Acceptor::stop()
{
    m_stopped.store(true);
}

void Acceptor::accept()
{
    m_acceptor->async_accept(
        std::bind(&Acceptor::onAccept, this, std::placeholders::_1, std::placeholders::_2));
}

void Acceptor::onAccept(const boost::system::error_code &err, boost::asio::ip::tcp::socket socket)
{
    if (!err) {
        auto connection = m_connectionManager->createConnection(std::move(socket), m_logger);
        connection->start();
    }
    else {
        m_logger->error("Failed to accept a new connection: {}", err.message());
    }

    if (m_stopped.load()) {
        m_acceptor->close();
        return;
    }

    accept();
}

} // namespace trogondb
