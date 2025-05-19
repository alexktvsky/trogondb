#include "trogondb/logging/level.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>

namespace trogondb {
namespace logging {

spdlog::level::level_enum translateLevel(Level level)
{
    switch (level) {
    case Level::TRACE:
        return spdlog::level::trace;
    case Level::DEBUG:
        return spdlog::level::debug;
    case Level::INFO:
        return spdlog::level::info;
    case Level::WARN:
        return spdlog::level::warn;
    case Level::ERROR:
        return spdlog::level::err;
    case Level::CRITICAL:
        return spdlog::level::critical;
    case Level::OFF:
        return spdlog::level::off;
    default:
        throw UnknownLevelException("Unknown logging level");
    }
}

Level translateLevel(spdlog::level::level_enum level)
{
    switch (level) {
    case spdlog::level::level_enum::trace:
        return Level::TRACE;
    case spdlog::level::level_enum::debug:
        return Level::DEBUG;
    case spdlog::level::level_enum::info:
        return Level::INFO;
    case spdlog::level::level_enum::warn:
        return Level::WARN;
    case spdlog::level::level_enum::err:
        return Level::ERROR;
    case spdlog::level::level_enum::critical:
        return Level::CRITICAL;
    case spdlog::level::level_enum::off:
        return Level::OFF;
    default:
        throw UnknownLevelException("Unknown logging level");
    }
}

Level getLevelByName(const std::string &name)
{
    static const std::map<std::string, Level> levelNames {
        {"trace", Level::TRACE},
        {"debug", Level::DEBUG},
        {"info", Level::INFO},
        {"warn", Level::WARN},
        {"error", Level::ERROR},
        {"critical", Level::CRITICAL},
        {"off", Level::OFF}};

    auto iter = levelNames.find(name);
    if (iter == levelNames.end()) {
        throw UnknownLevelException(fmt::format("Unknown logging level name '{}'", name));
    }
    return iter->second;
}

} // namespace logging
} //namespace trogondb
