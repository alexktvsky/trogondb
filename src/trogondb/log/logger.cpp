#include "trogondb/log/logger.h"

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace trogondb {
namespace log {

Logger::Logger(const std::string &name, const std::vector<std::shared_ptr<Handler>> &handlers)
{
    spdlog::init_thread_pool(LOG_QUEUE_SIZE, LOG_THREAD_COUNT);

    auto threadPool = spdlog::thread_pool();
    auto overflowPolicy = spdlog::async_overflow_policy::block;

    std::vector<std::shared_ptr<HandlerImpl>> sinks;

    for (const auto &handler : handlers) {
        sinks.push_back(handler->m_impl);
    }

    m_impl = std::make_shared<LoggerImpl>(name, sinks.begin(), sinks.end(), threadPool, overflowPolicy);

    spdlog::register_logger(m_impl);
}

std::string Logger::getName() const
{
    return m_impl->name();
}

void Logger::log(Level level, const std::string &msg)
{
    m_impl->log(translateLevel(level), msg);
}

void Logger::setFlushLevel(Level level)
{
    m_impl->flush_on(translateLevel(level));
}

void Logger::flush()
{
    m_impl->flush();
}

} // namespace log
} // namespace trogondb
