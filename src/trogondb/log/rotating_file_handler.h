#pragma once

#include <cstddef>

#include "trogondb/log/handler.h"

namespace trogondb {
namespace log {

class RotatingFileHandler : public Handler {
public:
    RotatingFileHandler(const std::string &fileName, size_t maxFileSize, size_t maxFiles, bool rotateOnOpen = false);
};

} // namespace log
} // namespace trogondb
