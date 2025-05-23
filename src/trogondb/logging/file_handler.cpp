#include "trogondb/logging/file_handler.h"

#include <spdlog/sinks/basic_file_sink.h>

namespace trogondb {
namespace logging {

FileHandler::FileHandler(const std::string &fileName)
    : Handler(std::make_shared<spdlog::sinks::basic_file_sink_mt>(fileName))
{
    // ...
}

} // namespace logging
} // namespace trogondb
