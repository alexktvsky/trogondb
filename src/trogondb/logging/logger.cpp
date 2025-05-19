#include "trogondb/logging/logger.h"

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace trogondb {
namespace logging {

Logger::Logger(const std::list<std::shared_ptr<Handler>> &handlers)
    : Logger(handlers, "")
{
    // ...
}

Logger::Logger(const std::list<std::shared_ptr<Handler>> &handlers, const std::string &name)
{
    // Queue with 8k items and 1 backing thread
    spdlog::init_thread_pool(8192, 1);

    auto threadPool = spdlog::thread_pool();
    auto overflowPolicy = spdlog::async_overflow_policy::block;

    std::list<std::shared_ptr<HandlerImpl>> sinks;

    for (const auto &handler : handlers) {
        sinks.push_back(handler->m_impl);
    }

    m_impl = std::make_shared<spdlog::async_logger>(name, sinks.begin(), sinks.end(), threadPool, overflowPolicy);

    // Flush all messages
    m_impl->flush_on(translateLevel(Level::TRACE));

    spdlog::register_logger(m_impl);
}

void Logger::log(Level level, const std::string &msg)
{
    m_impl->log(translateLevel(level), msg);
}

void Logger::addHandler(const std::shared_ptr<Handler> &handler)
{
    m_handlers.push_back(handler);
}

void Logger::flush()
{
    m_impl->flush();
}

std::string Logger::getName() const
{
    return m_impl->name();
}

} // namespace logging
} // namespace trogondb
