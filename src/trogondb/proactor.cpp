#include "trogondb/proactor.h"

namespace trogondb {

Proactor::Proactor()
    : m_io(std::make_shared<boost::asio::io_context>())
{}

void Proactor::run()
{
    m_io->run();
}

void Proactor::stop()
{
    m_io->stop();
}

bool Proactor::isRunning() const
{
    return !m_io->stopped();
}

std::shared_ptr<boost::asio::io_context> Proactor::getImpl()
{
    return m_io;
}

} // namespace trogondb
