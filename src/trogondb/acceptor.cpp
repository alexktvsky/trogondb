#include "trogondb/acceptor.h"

namespace trogondb {

Acceptor::Acceptor(std::shared_ptr<Proactor> proactor)
    : m_acceptor(std::make_shared<boost::asio::ip::tcp::acceptor>(*proactor->getImpl()))
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
}

} // namespace trogondb
