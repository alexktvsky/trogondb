#pragma once

#include <string>
#include <utility>

namespace trogondb {

class CommandResult {
public:

    CommandResult(const std::string &output, bool ok);

    CommandResult(const CommandResult &other);

    CommandResult(CommandResult &&other) noexcept;

    CommandResult &operator=(const CommandResult &other);

    CommandResult &operator=(CommandResult &&other) noexcept;

    static CommandResult value(const std::string &output);

    static CommandResult error(const std::string &output);

    std::string output;
    bool ok;
};

} // namespace trogondb
