#pragma once

#include <cstdint>
#include <string>
#include <map>

#include "trogondb/exception.h"

namespace spdlog {
namespace level {

enum level_enum : int;

} // namespace level
} // namespace spdlog

namespace trogondb {
namespace logging {

enum class Level : uint8_t {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    CRITICAL,
    OFF
};

spdlog::level::level_enum translateLevel(Level level);
Level translateLevel(spdlog::level::level_enum level);
Level getLevelByName(const std::string &name);

class UnknownLevelException : public Exception {
public:
    using Exception::Exception;
};

} // namespace logging
} //namespace trogondb
