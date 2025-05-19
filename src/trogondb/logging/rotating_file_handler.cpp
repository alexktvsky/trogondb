#include "trogondb/logging/rotating_file_handler.h"

#include <spdlog/sinks/rotating_file_sink.h>

namespace trogondb {
namespace logging {

RotatingFileHandler::RotatingFileHandler(const std::string &filename, size_t maxFileSize, size_t maxFiles, bool rotateOnOpen)
    : Handler(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filename, maxFileSize, maxFiles, rotateOnOpen))
{
    // ...
}

} // namespace logging
} // namespace trogondb
