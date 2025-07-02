#pragma once

#include <string>

#include "trogondb/cmd/command.h"

namespace trogondb {
namespace cmd {

class PingCommand : public ICommand {
public:
    CommandResult execute() override;
};

} // namespace cmd
} // namespace trogondb
