#include "compiler.h"
#include "signal.h"
#include <hydrogen/thread.h>
#include <hydrogen/types.h>

EXPORT int sigpending(sigset_t *set) {
    *set = hydrogen_thread_sigpending();
    return 0;
}
