#include "trogondb/os/process.h"

extern "C" {
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <sys/resource.h>
#include <sys/types.h>
}
#include <cstdlib>
#include <fmt/format.h>

#include "trogondb/os/system_exception.h"

namespace trogondb {
namespace os {

constexpr size_t MAX_PATH_LEN = 4096;

void Process::becomeDaemon()
{
    // Fork off the parent process
    pid_t pid = fork();

    if (pid < 0) {
        throw SystemException("Failed to become daemon, fork() failed");
    }

    // Parent terminates
    if (pid != 0) {
        exit(0);
    }

    // Become session leader
    if (setsid() < 0) {
        throw SystemException("Failed to become daemon, setsid() failed");
    }

    // Catch, ignore and handle signals
    // TODO: Implement a working signal handler
    signal(SIGHUP, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    // Fork off for the second time
    pid = fork();

    if (pid < 0) {
        throw SystemException("Failed to become daemon, fork() failed");
    }

    // 1th child terminates
    if (pid != 0) {
        exit(0);
    }

    // Close file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Redirect stdin, stdout, and stderr to /dev/null
    open("/dev/null", O_RDONLY);
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);
}

int Process::getPid()
{
    return getpid();
}

int Process::getPriority()
{
    return getpriority(PRIO_PROCESS, getPid());
}

void Process::setPriority(int priority)
{
    if (setpriority(PRIO_PROCESS, getPid(), priority) == -1) {
        throw SystemException(fmt::format("Failed to set a priority of process, setpriority() failed"));
    }
}

void Process::setUser(const std::string &user)
{
    // Not root, OK
    if (geteuid() != 0) {
        return;
    }

    struct passwd *pwd = getpwnam(user.c_str());
    if (!pwd) {
        throw SystemException(fmt::format("Failed to change user, getpwnam({}) failed", user));
    }

    unsigned int uid = pwd->pw_uid;
    if (setuid(uid) == -1) {
        throw SystemException(fmt::format("Failed to change user, setuid({}) failed", uid));
    }
}

void Process::setGroup(const std::string &group)
{
    // Not root, OK
    if (getegid() != 0) {
        return;
    }

    struct group *grp = getgrnam(group.c_str());
    if (!grp) {
        throw SystemException(fmt::format("Failed to change group, getgrnam({}) failed", group));
    }

    unsigned int gid = grp->gr_gid;
    if (setgid(gid) == -1) {
        throw SystemException(fmt::format("Failed to change group, setgid({}) failed", gid));
    }
}

std::string Process::getWorkingDirectory()
{
    char cwd[MAX_PATH_LEN];

    if (!getcwd(cwd, sizeof(cwd))) {
        throw SystemException("Failed to get working directory");
    }

    return std::string(cwd);
}

void Process::setWorkingDirectory(const std::string &workdir)
{
    if ((chdir(workdir.c_str())) < 0) {
        throw SystemException(fmt::format("Failed to change working directory, chdir({}) failed", workdir));
    }
}

} // namespace os
} // namespace trogondb
