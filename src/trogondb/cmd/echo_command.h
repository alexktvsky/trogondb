#pragma once

#include <string>

#include "trogondb/cmd/command.h"

namespace trogondb {
namespace cmd {

class EchoCommand : public ICommand {
public:
    CommandResult execute(const std::vector<std::string> &args) override;
};

} // namespace cmd
} // namespace trogondb
