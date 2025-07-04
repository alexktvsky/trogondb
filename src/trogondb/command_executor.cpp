#include "trogondb/command_executor.h"

#include "trogondb/log/log_manager.h"
#include "trogondb/cmd/ping_command.h"
#include "trogondb/cmd/echo_command.h"
#include "trogondb/cmd/get_command.h"
#include "trogondb/cmd/set_command.h"

namespace trogondb {

CommandExecutor::CommandExecutor(std::shared_ptr<KeyValueStore> store)
    : m_logger(log::LogManager::instance().getDefaultLogger())
    , m_store(store)
    , m_commands()
{
    m_commands["ping"] = std::make_shared<cmd::PingCommand>();
    m_commands["echo"] = std::make_shared<cmd::EchoCommand>();
    m_commands["get"] = std::make_shared<cmd::EchoCommand>();
    m_commands["set"] = std::make_shared<cmd::EchoCommand>();
}

cmd::CommandResult CommandExecutor::execute(const std::string &commandName, const std::vector<std::string> &args)
{
    std::shared_ptr<cmd::ICommand> cmd;




    // else {
    //     return cmd::CommandResult::error(fmt::format("unknown command '{}', with args beginning with: \r\n", commandName, args[0]));
    // }

    return cmd->execute(args);
}

} // namespace trogondb
