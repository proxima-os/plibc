#include "compiler.h"
#include "signal.h"
#include <hydrogen/thread.h>

EXPORT int sigsuspend(const sigset_t *sigmask) {
    return hydrogen_thread_sigsuspend(*sigmask);
}
