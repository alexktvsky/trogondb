#pragma once

#include <cstddef>

#include "trogondb/logging/handler.h"

namespace trogondb {
namespace logging {

class FileHandler : public Handler {
public:
    FileHandler(const std::string &fileName);
};

} // namespace logging
} // namespace trogondb
