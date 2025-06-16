#include "trogondb/log/rotating_file_handler.h"

#include <spdlog/sinks/rotating_file_sink.h>

namespace trogondb {
namespace log {

RotatingFileHandler::RotatingFileHandler(const std::string &fileName, size_t maxFileSize, size_t maxFiles, bool rotateOnOpen)
    : Handler(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(fileName, maxFileSize, maxFiles, rotateOnOpen))
{}

} // namespace log
} // namespace trogondb
