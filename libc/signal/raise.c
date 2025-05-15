#include "compiler.h"
#include "signal.h"
#include <hydrogen/process.h>
#include <unistd.h>

EXPORT int raise(int sig) {
    return hydrogen_process_send_signal(HYDROGEN_THIS_PROCESS, sig);
}
