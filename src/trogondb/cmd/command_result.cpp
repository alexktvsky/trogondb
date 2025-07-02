#include "trogondb/cmd/command_result.h"

namespace trogondb {
namespace cmd {

CommandResult::CommandResult(const std::string &output, bool ok)
    : output(output)
    , ok(ok)
{}

CommandResult::CommandResult(const CommandResult &other)
    : output(other.output)
    , ok(other.ok)
{}

CommandResult::CommandResult(CommandResult &&other) noexcept
    : output(std::move(other.output))
    , ok(other.ok)
{}

CommandResult &CommandResult::operator=(const CommandResult& other)
{
    if (this != &other) {
        output = other.output;
        ok = other.ok;
    }
    return *this;
}

CommandResult &CommandResult::operator=(CommandResult &&other) noexcept
{
    if (this != &other) {
        output = std::move(other.output);
        ok = other.ok;
    }
    return *this;
}

CommandResult CommandResult::value(const std::string &output)
{
    return {output, true};
}

CommandResult CommandResult::error(const std::string &output)
{
    return {output, false};
}

} // namespace cmd
} // namespace trogondb
