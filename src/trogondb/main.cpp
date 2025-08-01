#include <memory>
#include <vector>
#include <fmt/base.h>

#include "trogondb/command_line_parser.h"
#include "trogondb/config_parser.h"
#include "trogondb/os/process.h"
#include "trogondb/log/log_manager.h"
#include "trogondb/log/file_handler.h"
#include "trogondb/log/stream_handler.h"
#include "trogondb/log/rotating_file_handler.h"
#include "trogondb/proactor.h"
#include "trogondb/server.h"
#include "trogondb/signal_dispatcher.h"
#include "trogondb/platform_defines.h"
#include "trogondb/release.h"

const std::string DEFAULT_CONFIG_FILENAME = "/etc/trogondb/trogondb.yml";

namespace trogondb {

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

CommandLineParser parseArgs(const std::vector<std::string> &args)
{
    CommandLineParser commandLineParser;
    commandLineParser.addOption("h", "help", false);
    commandLineParser.addOption("v", "version", false);
    commandLineParser.addOption("c", "config", true);

    commandLineParser.parse(args);

    return commandLineParser;
}

void initializeProcess(const std::shared_ptr<Config> &config)
{
    if (config->daemon) {
        os::Process::becomeDaemon();
    }

    if (!config->workdir.empty()) {
        os::Process::setWorkingDirectory(config->workdir);
    }

    if (config->priority != 0) {
        os::Process::setPriority(config->priority);
    }

    if (!config->user.empty()) {
        os::Process::setUser(config->user);
    }

    if (!config->group.empty()) {
        os::Process::setGroup(config->group);
    }
}

std::shared_ptr<log::Logger> createLogger(const std::shared_ptr<Config> &config)
{
    std::vector<std::shared_ptr<log::Handler>> handlers;
    log::Level minLevel = log::Level::OFF;

    for (const auto &log : config->logs) {
        std::shared_ptr<log::Handler> handler;

        if (log.target == "stdout") {
            handler = std::make_shared<log::StreamHandler>(stdout);
        }
        else if (log.target == "stderr") {
            handler = std::make_shared<log::StreamHandler>(stderr);
        }
        else if (log.limit != 0) {
            handler = std::make_shared<log::RotatingFileHandler>(log.target, log.limit, log.rotate);
        }
        else {
            handler = std::make_shared<log::FileHandler>(log.target);
        }

        log::Level handlerLevel = log::getLevelByName(log.level);
        handler->setLevel(handlerLevel);

        if (minLevel > handlerLevel) {
            minLevel = handlerLevel;
        }

        handlers.push_back(handler);
    }

    auto logger = std::make_shared<log::Logger>("", handlers);
    logger->setLevel(minLevel);

    return logger;
}

} // namespace trogondb

int main(int argc, char **argv)
{
    const std::vector<std::string> args(argv + 1, argv + argc);

    try {
        auto commandLineParser = trogondb::parseArgs(args);

        if (commandLineParser.hasOption("help")) {
            trogondb::printHelpInfo();
            return 0;
        }
        if (commandLineParser.hasOption("version")) {
            trogondb::printVersionInfo();
            return 0;
        }

        std::string configFilename = DEFAULT_CONFIG_FILENAME;
        if (commandLineParser.hasOption("config")) {
            configFilename = commandLineParser.getOptionValue("config");
        }

        auto config = trogondb::ConfigParser::parseFile(configFilename);

        trogondb::initializeProcess(config);

        auto logger = trogondb::createLogger(config);
        trogondb::log::LogManager::instance().setDefaultLogger(logger);

        auto proactor = std::make_shared<trogondb::Proactor>();
        auto server = std::make_shared<trogondb::Server>(proactor, config);

        auto signalDispatcher = std::make_shared<trogondb::SignalDispatcher>(proactor);
        // TODO: add handlers
        // signalDispatcher->addHandler(SIGTERM, std::make_shared<trogondb::SignalHandlerForShutdownRequest>(server));
        // signalDispatcher->addHandler(SIGINT, std::make_shared<trogondb::SignalHandlerForShutdownRequest>(server));
        // signalDispatcher->addHandler(SIGHUP, std::make_shared<trogondb::SignalHandlerForConfigReload>(server));
        // signalDispatcher->addHandler(SIGQUIT, std::make_shared<trogondb::SignalHandlerForCrashExit>(server));

        server->start();
        signalDispatcher->start();

        proactor->run();
    }
    catch (const trogondb::CommandLineException &e) {
        fmt::print(stderr, "{}\n\n", e.what());
        trogondb::printHelpInfo();
        return 1;
    }
    catch (const trogondb::ConfigFileException &e) {
        fmt::print(stderr, "{}\n", e.what());
        return 1;
    }
    catch (const boost::system::system_error &e) {
        fmt::print(stderr, "{}\n", e.what());
        return 1;
    }
    catch (...) {
        // TODO: Add error log
        throw;
    }

    return 0;
}
