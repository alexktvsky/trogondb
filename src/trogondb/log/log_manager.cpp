#include "trogondb/log/log_manager.h"

namespace trogondb {
namespace log {

void LogManager::addLogger(std::shared_ptr<Logger> logger)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_loggers[logger->getName()] = logger;
}

std::shared_ptr<Logger> LogManager::getLogger(const std::string &name) const
{
    return m_loggers[name];
}

void LogManager::setDefaultLogger(std::shared_ptr<Logger> logger)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_defaultLogger = logger;
    m_loggers[logger->getName()] = logger;
}

std::shared_ptr<Logger> LogManager::getDefaultLogger() const
{
    return m_defaultLogger;
}

LogManager &LogManager::instance()
{
    static LogManager instance;
    return instance;
}

} // namespace log
} // namespace trogondb

