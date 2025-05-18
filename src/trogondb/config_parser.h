#pragma once

#include <string>
#include <memory>

#include "trogondb/server_config.h"
#include "trogondb/exception.h"

namespace trogondb {

class ConfigParser {
public:
    static std::shared_ptr<ServerConfig> parseFile(const std::string &filename);
};

class ConfigFileException : public Exception {
public:
    using Exception::Exception;
};

} // namespace trogondb
