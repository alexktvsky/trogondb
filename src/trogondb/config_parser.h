#pragma once

#include <string>
#include <memory>

#include "trogondb/config.h"
#include "trogondb/config_node.h"

namespace trogondb {

class ConfigParser {
public:
    static std::shared_ptr<Config> parseFile(const std::string &fileName);
};

} // namespace trogondb
