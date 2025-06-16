#pragma once

#include <cstddef>

#include "trogondb/log/handler.h"

namespace trogondb {
namespace log {

class FileHandler : public Handler {
public:
    FileHandler(const std::string &fileName);
};

} // namespace log
} // namespace trogondb
