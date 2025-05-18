#pragma once

#include <string>
#include <list>
#include <memory>
#include <fmt/format.h>

#include "trogondb/logging/handler.h"
#include "trogondb/logging/level.h"

namespace spdlog {
class async_logger;
} // namespace spdlog

namespace trogondb {
namespace logging {

class Logger {
public:
    Logger(const std::list<std::shared_ptr<Handler>> &handlers);
    Logger(const std::list<std::shared_ptr<Handler>> &m_handlers, const std::string &name);

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
    void addHandler(const std::shared_ptr<Handler> &handler);
    void flush();
    std::string getName() const;
private:
    using LoggerImpl = spdlog::async_logger;
    std::shared_ptr<LoggerImpl> m_impl;
    std::list<std::shared_ptr<Handler>> m_handlers;
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

} // namespace logging
} // namespace trogondb
