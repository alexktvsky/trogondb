#include "command_line_parser.h"

#include <fmt/format.h>

namespace trogondb {

CommandLineParser::CommandLineParser() {}

void CommandLineParser::addOption(const std::string &opt, const std::string &longOpt, bool hasArg)
{
    m_options.push_back(CommandLineOption {opt, longOpt, hasArg});
}

void CommandLineParser::addOption(const std::string &longOpt, bool hasArg)
{
    addOption("", longOpt, hasArg);
}

void CommandLineParser::parse(const std::vector<std::string> &args)
{
    std::vector<std::string>::const_iterator iter;

    for (iter = args.begin(); iter != args.end(); ++iter) {
        std::string opt = *iter;
        bool found = false;

        if (opt[0] != '-') {
            m_parsedArguments.push_back(opt);
            continue;
        }

        for (const CommandLineOption &option : m_options) {
            if (opt == ("-" + option.opt) || opt == ("--" + option.longOpt)) {
                if (option.hasArg) {
                    ++iter;
                    if (iter == args.end()) {
                        throw CommandLineException(fmt::format("No argument found for option '{}'", opt));
                    }
                    m_parsedOptions.insert({option.longOpt, *iter});
                }
                else {
                    m_parsedOptions.insert({option.longOpt, "true"});
                }

                found = true;
                break;
            }
        }
        if (!found) {
            throw CommandLineException(fmt::format("Unknown argument '{}'", opt));
        }
    }
}

bool CommandLineParser::hasOption(const std::string &longOpt) const
{
    return m_parsedOptions.find(longOpt) != m_parsedOptions.end();
}

std::string CommandLineParser::getOptionValue(const std::string &longOpt) const
{
    return m_parsedOptions.find(longOpt)->second;
}

std::vector<std::string> CommandLineParser::getArgs() const
{
    return m_parsedArguments;
}

} // namespace trogondb
