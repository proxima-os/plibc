#include "compiler.h"
#define _PLIBC_INLINE_IMPL EXPORT
#include "signal.h"
#include <unistd.h>

EXPORT __sig_handler_t signal(int sig, __sig_handler_t func) {
    struct sigaction sa = {
            .sa_handler = func,
    };

    struct sigaction old;

    if (sigaction(sig, &sa, &old)) {
        return SIG_ERR;
    }

    return old.sa_handler;
}

EXPORT int raise(int sig) {
    return kill(getpid(), sig);
}
