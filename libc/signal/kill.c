#include "compiler.h"
#include "signal.h"
#include <errno.h>
#include <hydrogen/handle.h>
#include <hydrogen/process.h>
#include <hydrogen/thread.h>
#include <hydrogen/types.h>
#include <sys/types.h>
#include <unistd.h>

EXPORT int kill(pid_t pid, int sig) {
    if (pid > 0) {
        if (pid == getpid()) {
            int error = hydrogen_process_send_signal(HYDROGEN_THIS_PROCESS, sig);

            if (unlikely(error)) {
                errno = error;
                return -1;
            }

            return 0;
        }

        hydrogen_ret_t ret = hydrogen_process_find(pid, 0);

        if (unlikely(ret.error)) {
            errno = ret.error;
            return -1;
        }

        int error = hydrogen_process_send_signal(ret.integer, sig);
        hydrogen_namespace_remove(HYDROGEN_THIS_NAMESPACE, ret.integer);

        if (unlikely(error)) {
            errno = error == EBADF ? EPERM : error;
            return -1;
        }

        return 0;
    } else if (pid == (pid_t)-1) {
        int error = hydrogen_process_send_signal(HYDROGEN_INVALID_HANDLE, sig);

        if (unlikely(error)) {
            errno = error;
            return -1;
        }

        return 0;
    } else {
        if (pid == 0) pid = -getpgrp();

        int error = hydrogen_process_group_send_signal(-pid, sig);

        if (unlikely(error)) {
            errno = error;
            return -1;
        }

        return 0;
    }
}
