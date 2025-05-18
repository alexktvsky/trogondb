#pragma once

#include <string>
#include <memory>

#include "trogondb/logging/level.h"

namespace spdlog {
namespace sinks {
class sink;
} // namespace sinks
} // namespace spdlog

namespace trogondb {
namespace logging {

using HandlerImpl = spdlog::sinks::sink;

class Logger;

class Handler {
    friend class Logger;
public:
    logging::Level getLevel() const;
    void setLevel(logging::Level level);
    void setPattern(const std::string &pattern);
    void flush();
protected:
    Handler(const std::shared_ptr<HandlerImpl> &impl);
private:
    std::shared_ptr<HandlerImpl> m_impl;
};

} // namespace logging
} // namespace trogondb
