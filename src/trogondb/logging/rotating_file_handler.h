#pragma once

#include <cstddef>

#include "trogondb/logging/handler.h"

namespace trogondb {
namespace logging {

class RotatingFileHandler : public Handler {
public:
    RotatingFileHandler(const std::string &fileName, size_t maxFileSize, size_t maxFiles, bool rotateOnOpen = false);
};

} // namespace logging
} // namespace trogondb
