#pragma once

#include "trogondb/cmd/command_result.h"

namespace trogondb {
namespace cmd {

class ICommand {
public:
    virtual CommandResult execute() = 0;
    virtual ~ICommand() = default;
};

} // namespace cmd
} // namespace trogondb
