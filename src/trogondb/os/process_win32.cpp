#include "trogondb/os/process.h"

#include <windows.h>
#include <direct.h>
#include <io.h>
#include <fcntl.h>

#include "trogondb/os/system_exception.h"

namespace trogondb {
namespace os {

void Process::becomeDaemon()
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    if (FreeConsole() == 0) {
        throw SystemException("Failed to FreeConsole()");
    }

    _close(STDIN_FILENO);
    _close(STDOUT_FILENO);
    _close(STDERR_FILENO);
}

int Process::getPid()
{
    return static_cast<int>(GetCurrentProcessId());
}

int Process::getPriority()
{
    DWORD priority = GetPriorityClass(GetCurrentProcess());
    if (!priority) {
        throw SystemException("Failed to get process priority");
    }

    // Map to Unix-like values for consistency
    switch (priority) {
        case IDLE_PRIORITY_CLASS: return 19;
        case BELOW_NORMAL_PRIORITY_CLASS: return 10;
        case NORMAL_PRIORITY_CLASS: return 0;
        case ABOVE_NORMAL_PRIORITY_CLASS: return -5;
        case HIGH_PRIORITY_CLASS: return -10;
        case REALTIME_PRIORITY_CLASS: return -20;
        default: return 0;
    }
}

void Process::setPriority(int priority)
{
    DWORD winPriority;

    if (priority >= 15) {
        winPriority = IDLE_PRIORITY_CLASS;
    }
    else if (priority >= 5) {
        winPriority = BELOW_NORMAL_PRIORITY_CLASS;
    }
    else if (priority >= -4) {
        winPriority = NORMAL_PRIORITY_CLASS;
    }
    else if (priority >= -9) {
        winPriority = ABOVE_NORMAL_PRIORITY_CLASS;
    }
    else if (priority >= -19) {
        winPriority = HIGH_PRIORITY_CLASS;
    }
    else {
        winPriority = REALTIME_PRIORITY_CLASS;
    }

    if (!SetPriorityClass(GetCurrentProcess(), winPriority)) {
        throw SystemException("Failed to set process priority");
    }
}

std::string Process::getWorkingDirectory()
{
    char buffer[MAX_PATH];
    if (!_getcwd(buffer, MAX_PATH)) {
        throw SystemException("Failed to get working directory");
    }
    return std::string(buffer);
}

void Process::setWorkingDirectory(const std::string &workdir)
{
    if (!SetCurrentDirectory(workdir)) {
        throw SystemException("Failed to set working directory");
    }
}

void Process::setUser(const std::string &user)
{
    // ...
}

void Process::setGroup(const std::string &group)
{
    // ...
}

} // namespace os
} // namespace trogondb
