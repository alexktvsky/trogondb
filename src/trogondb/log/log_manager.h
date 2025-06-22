#pragma once

#include <string>
#include <memory>
#include <list>
#include <unordered_map>
#include <mutex>

#include "trogondb/log/logger.h"

namespace trogondb {
namespace log {

class LogManager {
public:
    void addLogger(std::shared_ptr<Logger> logger);

    std::shared_ptr<Logger> getLogger(const std::string &name) const;

    void setDefaultLogger(std::shared_ptr<Logger> logger);

    std::shared_ptr<Logger> getDefaultLogger() const;

    static LogManager& instance();
private:
    LogManager() = default;
    ~LogManager() = default;
    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;
    LogManager(LogManager&&) = delete;
    LogManager& operator=(LogManager&&) = delete;

    std::shared_ptr<Logger> m_defaultLogger;
    mutable std::unordered_map<std::string, std::shared_ptr<Logger>> m_loggers;
    mutable std::mutex m_mutex;
};

} // namespace log
} // namespace trogondb
