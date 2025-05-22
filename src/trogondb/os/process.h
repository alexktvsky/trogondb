#pragma once

#include <string>

namespace trogondb {
namespace os {

class Process {
public:
    Process() = delete;
    static void becomeDaemon();
    static int getPid();
    static int getPriority();
    static void setPriority(int priority);
    static void setUser(const std::string &user);
    static void setGroup(const std::string &group);
    static std::string getWorkingDirectory();
    static void setWorkingDirectory(const std::string &workdir);
};

} // namespace os
} // namespace trogondb
