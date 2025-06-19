#include "trogondb/proactor.h"

namespace trogondb {

Proactor::Proactor()
    : m_context(std::make_shared<boost::asio::io_context>())
{}

void Proactor::run()
{
    m_context->run();
}

void Proactor::stop()
{
    m_context->stop();
}

bool Proactor::isRunning() const
{
    return !m_context->stopped();
}

std::shared_ptr<boost::asio::io_context> Proactor::getImpl()
{
    return m_context;
}

} // namespace trogondb
