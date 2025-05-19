#include "trogondb/logging/file_handler.h"

#include <spdlog/sinks/basic_file_sink.h>

namespace trogondb {
namespace logging {

FileHandler::FileHandler(const std::string &filename)
    : Handler(std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename))
{
    // ...
}

} // namespace logging
} // namespace trogondb
