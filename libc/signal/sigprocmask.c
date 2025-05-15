#include "compiler.h"
#include "signal.h"
#include <hydrogen/thread.h>

EXPORT int sigprocmask(int how, const sigset_t *restrict set, sigset_t *restrict oset) {
    return hydrogen_thread_sigmask(how, set, oset);
}
