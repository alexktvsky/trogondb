#include "trogondb/log/file_handler.h"

#include <spdlog/sinks/basic_file_sink.h>

namespace trogondb {
namespace log {

FileHandler::FileHandler(const std::string &fileName)
    : Handler(std::make_shared<spdlog::sinks::basic_file_sink_mt>(fileName))
{}

} // namespace log
} // namespace trogondb
