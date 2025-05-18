#pragma once

#include <string>
#include <memory>

#include "trogondb/server_config.h"
#include "trogondb/config_node.h"

namespace trogondb {

class ConfigParser {
public:
    static std::shared_ptr<ServerConfig> parseFile(const std::string &fileName);
};

} // namespace trogondb
