#include "compiler.h"
#include "signal.h"
#include "signal.p.h"
#include <errno.h>
#include <hydrogen/thread.h>

EXPORT int raise(int sig) {
    if (sig == 0 || sig >= NUM_SIGNALS) {
        errno = EINVAL;
        return -1;
    }

    void (*handler)(int) = sighand[sig];

    if (handler == SIG_DFL) {
        hydrogen_thread_exit();
    } else if (handler != SIG_IGN) {
        sighand[sig] = SIG_DFL;
        handler(sig);
    }

    return 0;
}
