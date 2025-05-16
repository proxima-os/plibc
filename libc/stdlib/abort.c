#include "compiler.h"
#include "stdlib.h"
#include <hydrogen/process.h>
#include <hydrogen/thread.h>
#include <signal.h>

#undef abort

EXPORT void abort(void) {
    hydrogen_thread_send_signal(HYDROGEN_THIS_THREAD, SIGABRT);
    hydrogen_process_exit(127);
}
