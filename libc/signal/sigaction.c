#include "compiler.h"
#include "signal.h"
#include <hydrogen/process.h>

EXPORT int sigaction(int sig, const struct sigaction *restrict act, struct sigaction *restrict oact) {
    return hydrogen_process_sigaction(HYDROGEN_THIS_PROCESS, sig, &act->__base, &oact->__base);
}
