#include "trogondb/os/process.h"

#include "trogondb/platform_defines.h"

#if WIN32
    #include "trogondb/os/process_win32.h"
using ProcessImpl = trogondb::os::ProcessWin;
#else
    #include "trogondb/os/process_unix.h"
using ProcessImpl = trogondb::os::ProcessUnix;
#endif

namespace trogondb {
namespace os {

void Process::becomeDaemon()
{
    ProcessImpl::becomeDaemon();
}

int Process::getPid()
{
    return ProcessImpl::getPid();
}

int Process::getPriority()
{
    return ProcessImpl::getPriority();
}

void Process::setPriority(int priority)
{
    ProcessImpl::setPriority(priority);
}

void Process::setUser(const std::string &user)
{
    ProcessImpl::setUser(user);
}

void Process::setGroup(const std::string &group)
{
    ProcessImpl::setGroup(group);
}

std::string Process::getWorkingDirectory()
{
    return ProcessImpl::getWorkingDirectory();
}

void Process::setWorkingDirectory(const std::string &workdir)
{
    ProcessImpl::setWorkingDirectory(workdir);
}

} // namespace os
} // namespace trogondb
