#pragma once

#include <string>
#include <vector>

#include "trogondb/cmd/command_result.h"

namespace trogondb {
namespace cmd {

class ICommand {
public:
    virtual CommandResult execute(const std::vector<std::string> &args) = 0;
    virtual ~ICommand() = default;
};

} // namespace cmd
} // namespace trogondb
