#pragma once

#include <cstddef>
#include <cstdint>
#include <list>
#include <string>

namespace trogondb {

struct Config {
    std::string host;
    uint16_t port;
    bool daemon;
    std::string workdir;
    int priority;
    std::string user;
    std::string group;

    struct LogHandlerConfig {
        std::string target;
        std::string level;
        size_t limit;
        unsigned int rotate;
    };
    std::list<LogHandlerConfig> logs;
};

} // namespace trogondb
