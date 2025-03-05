#include "posix/unistd.h"
#include "compiler.h"
#include "stub.h"
#include <errno.h>
#include <hydrogen/handle.h>
#include <hydrogen/log.h>
#include <hydrogen/thread.h>
#include <limits.h>
#include <sys/types.h>

hydrogen_handle_t log_handle;

EXPORT pid_t fork(void) {
    STUB();
    return -1;
}

EXPORT int execl(const char *path, const char *arg, ...) {
    STUB();
    return -1;
}

EXPORT int execv(const char *path, char *const argv[]) {
    STUB();
    return -1;
}

EXPORT int execle(const char *path, const char *arg, ...) {
    STUB();
    return -1;
}

EXPORT int execve(const char *path, char *const argv[], char *const envp[]) {
    STUB();
    return -1;
}

EXPORT int execlp(const char *file, const char *arg, ...) {
    STUB();
    return -1;
}

EXPORT int execvp(const char *file, char *const argv[]) {
    STUB();
    return -1;
}

EXPORT void _exit(int status) {
    // TODO: Use status code
    // TODO: Kill other threads
    hydrogen_thread_exit();
}

EXPORT unsigned alarm(unsigned seconds) {
    STUB();
    return 0;
}

EXPORT int pause(void) {
    STUB();
    return -1;
}

EXPORT unsigned sleep(unsigned seconds) {
    STUB();
    return -1;
}

EXPORT pid_t getpid(void) {
    // TODO (bad idea to use STUB() here because this is called in abort())
    return 0;
}

EXPORT pid_t getppid(void) {
    STUB();
    return 0;
}

EXPORT uid_t getuid(void) {
    STUB();
    return 0;
}

EXPORT uid_t geteuid(void) {
    STUB();
    return 0;
}

EXPORT gid_t getgid(void) {
    STUB();
    return 0;
}

EXPORT gid_t getegid(void) {
    STUB();
    return 0;
}

EXPORT int setuid(uid_t uid) {
    STUB();
    return -1;
}

EXPORT int setgid(gid_t gid) {
    STUB();
    return -1;
}

EXPORT int getgroups(int gidsetsize, gid_t grouplist[]) {
    STUB();
    return -1;
}

EXPORT char *getlogin(void) {
    STUB();
    return NULL;
}

EXPORT pid_t getpgrp(void) {
    STUB();
    return 0;
}

EXPORT pid_t setsid(void) {
    STUB();
    return -1;
}

EXPORT int setpgid(pid_t pid, pid_t pgid) {
    STUB();
    return -1;
}

EXPORT char *ttyname(int fildes) {
    STUB();
    return NULL;
}

EXPORT int isatty(int fildes) {
    // TODO
    return fildes == STDOUT_FILENO || fildes == STDERR_FILENO;
}

EXPORT long sysconf(int name) {
    STUB();
    return -1;
}

EXPORT int chdir(const char *path) {
    STUB();
    return -1;
}

EXPORT char *getcwd(char *buf, size_t size) {
    STUB();
    return NULL;
}

EXPORT int link(const char *existing, const char *new) {
    STUB();
    return -1;
}

EXPORT int unlink(const char *path) {
    STUB();
    return -1;
}

EXPORT int rmdir(const char *path) {
    STUB();
    return -1;
}

EXPORT int access(const char *path, int amode) {
    STUB();
    return -1;
}

EXPORT int chown(const char *path, uid_t owner, gid_t group) {
    STUB();
    return -1;
}

EXPORT long pathconf(const char *path, int name) {
    STUB();
    return -1;
}

EXPORT long fpathconf(int fildes, int name) {
    STUB();
    return -1;
}

EXPORT int pipe(int fildes[2]) {
    STUB();
    return -1;
}

EXPORT int dup(int fildes) {
    STUB();
    return -1;
}

EXPORT int dup2(int fildes) {
    STUB();
    return -1;
}

EXPORT int close(int fildes) {
    STUB();
    return -1;
}

EXPORT ssize_t read(int fildes, void *buf, size_t nbyte) {
    STUB();
    return -1;
}

EXPORT ssize_t write(int fildes, const void *buf, size_t nbyte) {
    if (nbyte == 0) return 0;
    if (nbyte > SSIZE_MAX) nbyte = SSIZE_MAX;

    // TODO

    if ((fildes == STDOUT_FILENO || fildes == STDERR_FILENO) && log_handle) {
        int error = hydrogen_log_write(log_handle, buf, nbyte);

        if (unlikely(error)) {
            errno = error;
            return -1;
        }

        return nbyte;
    }

    errno = ENOSYS;
    return -1;
}

EXPORT off_t lseek(int fildes, off_t offset, int whence) {
    STUB();
    return -1;
}

EXPORT pid_t tcgetpgrp(int fildes) {
    STUB();
    return -1;
}

EXPORT int tcsetpgrp(int fildes, pid_t pgrp_id) {
    STUB();
    return -1;
}
