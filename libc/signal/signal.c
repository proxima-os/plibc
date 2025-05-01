#define __PLIBC_SIGNAL_INLINE
#include "signal.h"
#include "compiler.h"

EXPORT __sig_handler_t signal(int sig, __sig_handler_t func) {
    struct sigaction act = {.sa_handler = func, .sa_flags = 0};
    struct sigaction oact;

    if (sigaction(sig, &act, &oact)) return SIG_ERR;

    return oact.sa_handler;
}
