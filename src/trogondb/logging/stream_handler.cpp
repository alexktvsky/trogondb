#include "trogondb/logging/stream_handler.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace trogondb {
namespace logging {

StreamHandler::StreamHandler(std::FILE *stream)
    : Handler(createStreamHandler(stream))
{
    // ...
}

std::shared_ptr<HandlerImpl> StreamHandler::createStreamHandler(std::FILE *stream)
{
    if (stream == stdout) {
        return std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    }

    if (stream == stderr) {
        return std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
    }

    throw InvalidStreamException("Invalid stream");
}

} // namespace logging
} // namespace trogondb
