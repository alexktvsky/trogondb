#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "trogondb/exception.h"

namespace trogondb {

class CommandLineParser {
public:
    CommandLineParser();
    void addOption(const std::string &opt, const std::string &longOpt, bool hasArg);
    void addOption(const std::string &longOpt, bool hasArg);
    void parse(const std::vector<std::string> &args);
    bool hasOption(const std::string &opt) const;
    std::string getOptionValue(const std::string &opt) const;
    std::vector<std::string> getArgs() const;
private:
    struct CommandLineOption {
        const std::string opt;
        const std::string longOpt;
        const bool hasArg;
    };

    std::vector<CommandLineOption> m_options;
    std::vector<std::string> m_parsedArguments;
    std::unordered_map<std::string, std::string> m_parsedOptions;
};

class CommandLineException : public Exception {
public:
    using Exception::Exception;
};

} // namespace trogondb
