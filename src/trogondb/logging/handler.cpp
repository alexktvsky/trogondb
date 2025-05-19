#include "trogondb/logging/handler.h"

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace trogondb {
namespace logging {

Handler::Handler(const std::shared_ptr<HandlerImpl> &impl)
    : m_impl(impl)
{
    // ...
}

void Handler::setLevel(Level level)
{
    m_impl->set_level(translateLevel(level));
}

Level Handler::getLevel() const
{
    return translateLevel(m_impl->level());
}

void Handler::setPattern(const std::string &pattern)
{
    m_impl->set_pattern(pattern);
}

void Handler::flush()
{
    m_impl->flush();
}

} // namespace logging
} // namespace trogondb
