#include <memory>
#include <vector>
#include <fmt/base.h>

#include "trogondb/command_line_parser.h"
#include "trogondb/config_parser.h"
#include "trogondb/proactor.h"
#include "trogondb/server.h"
#include "trogondb/log/log_manager.h"
#include "trogondb/platform_defines.h"
#include "trogondb/release.h"

const std::string DEFAULT_CONFIG_FILENAME = "/etc/trogondb/trogondb.yml";

void printVersionInfo()
{
    fmt::print("{} {} {}\n", PROJECT_NAME, PROJECT_VERSION, PROJECT_BUILD_DATE);
    fmt::print("Target system: {} {}\n", SYSTEM_NAME, ARCH_NAME);
#ifdef COMPILER_NAME_VERSIONED
    fmt::print("Built by {}\n", COMPILER_NAME_VERSIONED);
#endif
}

void printHelpInfo()
{
    fmt::print(
        "Usage: {} [options...] [argments...]\n"
        "{} {} {}\n\n"
        "Options:\n"
        "  -h, --help                     Displays this message.\n"
        "  -v, --version                  Displays version information.\n"
        "  -c, --config <file>            Specify configuration file.\n",
        PROJECT_NAME_CMD_LINE,
        PROJECT_NAME,
        PROJECT_VERSION,
        PROJECT_BUILD_DATE);
}

trogondb::CommandLineParser parseArgs(const std::vector<std::string> &args)
{
    trogondb::CommandLineParser commandLineParser;
    commandLineParser.addOption("h", "help", false);
    commandLineParser.addOption("v", "version", false);
    commandLineParser.addOption("c", "config", true);

    commandLineParser.parse(args);

    return commandLineParser;
}

int main(int argc, char **argv)
{
    const std::vector<std::string> args(argv + 1, argv + argc);

    try {
        auto commandLineParser = parseArgs(args);

        if (commandLineParser.hasOption("help")) {
            printHelpInfo();
            return 0;
        }
        if (commandLineParser.hasOption("version")) {
            printVersionInfo();
            return 0;
        }

        std::string configFilename = DEFAULT_CONFIG_FILENAME;
        if (commandLineParser.hasOption("config")) {
            configFilename = commandLineParser.getOptionValue("config");
        }

        auto config = trogondb::ConfigParser::parseFile(configFilename);

        auto proactor = std::make_shared<trogondb::Proactor>();

        auto server = std::make_shared<trogondb::Server>(proactor, std::move(config));

        trogondb::log::LogManager::instance().setDefaultLogger(server->getLogger());

        server->start();
    }
    catch (const trogondb::CommandLineException &e) {
        std::fprintf(stderr, "%s\n\n", e.what());
        printHelpInfo();
        return 1;
    }
    catch (const trogondb::ConfigFileException &e) {
        std::fprintf(stderr, "%s\n", e.what());
        return 1;
    }
    catch (...) {
        // TODO: Add error log
        throw;
    }

    return 0;
}
