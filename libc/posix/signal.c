#include "signal.h"
#include "compiler.h"
#include "stub.h"

EXPORT __pid_t kill(__pid_t __pid, int __sig) {
    STUB();
    return -1;
}

EXPORT int sigaction(int __sig, const struct sigaction *__restrict __act, struct sigaction *__restrict __oact) {
    STUB();
    return -1;
}

EXPORT int sigprocmask(int how, const sigset_t *restrict set, sigset_t *restrict oset) {
    STUB();
    return -1;
}

EXPORT int sigpending(sigset_t *set) {
    STUB();
    return -1;
}

EXPORT int sigsuspend(const sigset_t *set) {
    STUB();
    return -1;
}
