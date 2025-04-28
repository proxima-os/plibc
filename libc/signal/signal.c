#include "signal.h"
#include "compiler.h"
#include "signal.p.h"
#include <errno.h>

EXPORT void (*signal(int sig, void (*func)(int)))(int) {
    if (sig == 0 || sig >= NUM_SIGNALS) {
        errno = EINVAL;
        return SIG_ERR;
    }

    void (*old)(int) = sighand[sig];
    sighand[sig] = func;
    return old;
}
