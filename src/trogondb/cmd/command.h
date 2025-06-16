#pragma once

#include <string>

namespace trogondb {

class ICommand {
public:
    virtual std::string execute() = 0;
    virtual ~ICommand() = default;
};

} // namespace trogondb
