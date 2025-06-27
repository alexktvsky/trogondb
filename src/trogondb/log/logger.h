#pragma once

#include <string>
#include <vector>
#include <memory>
#include <fmt/format.h>

#include "trogondb/log/handler.h"
#include "trogondb/log/level.h"

namespace spdlog {
class async_logger;
} // namespace spdlog

namespace trogondb {
namespace log {

constexpr std::size_t LOG_QUEUE_SIZE = 8192;
constexpr std::size_t LOG_THREAD_COUNT = 1;

class Logger {
public:
    Logger(const std::string &name, const std::vector<std::shared_ptr<Handler>> &handlers);

    std::string getName() const;

    template <typename... T>
    void log(Level level, const std::string &fmt, T &&...args);

    template <typename... T>
    void trace(const std::string &fmt, T &&...args);

    template <typename... T>
    void debug(const std::string &fmt, T &&...args);

    template <typename... T>
    void info(const std::string &fmt, T &&...args);

    template <typename... T>
    void warn(const std::string &fmt, T &&...args);

    template <typename... T>
    void error(const std::string &fmt, T &&...args);

    template <typename... T>
    void critical(const std::string &fmt, T &&...args);

    void log(Level level, const std::string &msg);

    void setLevel(Level level);

    void setFlushLevel(Level level);

    void flush();

private:
    using LoggerImpl = spdlog::async_logger;
    std::shared_ptr<LoggerImpl> m_impl;
};

template <typename... T>
void Logger::log(Level level, const std::string &fmt, T &&...args)
{
    log(level, fmt::vformat(fmt, fmt::make_format_args(args...)));
}

template <typename... T>
void Logger::trace(const std::string &fmt, T &&...args)
{
    log(Level::TRACE, fmt, std::forward<T>(args)...);
}

template <typename... T>
void Logger::debug(const std::string &fmt, T &&...args)
{
    log(Level::DEBUG, fmt, std::forward<T>(args)...);
}

template <typename... T>
void Logger::info(const std::string &fmt, T &&...args)
{
    log(Level::INFO, fmt, std::forward<T>(args)...);
}

template <typename... T>
void Logger::warn(const std::string &fmt, T &&...args)
{
    log(Level::WARN, fmt, std::forward<T>(args)...);
}

template <typename... T>
void Logger::error(const std::string &fmt, T &&...args)
{
    log(Level::ERROR, fmt, std::forward<T>(args)...);
}

template <typename... T>
void Logger::critical(const std::string &fmt, T &&...args)
{
    log(Level::CRITICAL, fmt, std::forward<T>(args)...);
}

} // namespace log
} // namespace trogondb
