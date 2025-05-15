#include "compiler.h"
#include "sys/wait.h"
#include <errno.h>
#include <hydrogen/handle.h>
#include <hydrogen/process.h>
#include <hydrogen/types.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

EXPORT int waitpid(pid_t pid, int *stat_loc, int options) {
    unsigned flags = HYDROGEN_PROCESS_WAIT_EXITED | HYDROGEN_PROCESS_WAIT_KILLED | HYDROGEN_PROCESS_WAIT_DISCARD |
                     HYDROGEN_PROCESS_WAIT_UNQUEUE;
    uint64_t deadline = 0;

    if (options & WNOHANG) deadline = 1;
    if (options & WUNTRACED) flags |= HYDROGEN_PROCESS_WAIT_STOPPED;

    __siginfo_t info;

    if (pid > 0) {
        hydrogen_ret_t ret = hydrogen_process_find(pid, 0);

        if (unlikely(ret.error)) {
            if (ret.error == ESRCH) errno = ECHILD;
            else errno = ret.error;
            return -1;
        }

        int error = hydrogen_process_wait(ret.integer, flags, &info, deadline);
        hydrogen_namespace_remove(HYDROGEN_THIS_NAMESPACE, ret.integer);

        if (unlikely(error)) {
            if (error == EBADF) errno = ECHILD;
            else errno = error;
            return -1;
        }
    } else if (pid == (pid_t)-1) {
        hydrogen_ret_t ret = hydrogen_process_wait_id(0, flags, &info, deadline);

        if (unlikely(ret.error)) {
            if (ret.error == EAGAIN) errno = ECHILD;
            else errno = ret.error;
            return -1;
        }

        pid = ret.integer;
    } else {
        if (pid == 0) pid = -getpgrp();

        hydrogen_ret_t ret = hydrogen_process_wait_id(-pid, flags, &info, deadline);

        if (unlikely(ret.error)) {
            if (ret.error == EAGAIN) errno = ECHILD;
            else errno = ret.error;
            return -1;
        }

        pid = ret.integer;
    }

    switch (info.__code) {
    case __CLD_EXITED: *stat_loc = info.__data.__user_or_sigchld.__status & 0xff; break;
    case __CLD_STOPPED: *stat_loc = 0x7f00 | (info.__data.__user_or_sigchld.__status & 0x7f); break;
    case __CLD_CONTINUED: *stat_loc = 0xff00 | (info.__data.__user_or_sigchld.__status & 0x7f); break;
    default: *stat_loc = (info.__data.__user_or_sigchld.__status & 0x7f) << 8; break;
    }

    return pid;
}
