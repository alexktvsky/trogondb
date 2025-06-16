#pragma once

#include <string>

namespace trogondb {
namespace cmd {

class ICommand {
public:
    virtual std::string execute() = 0;
    virtual ~ICommand() = default;
};

} // namespace cmd
} // namespace trogondb
