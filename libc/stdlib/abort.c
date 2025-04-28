#include "compiler.h"
#include "stdlib.h"
#include <hydrogen/thread.h>
#include <signal.h>

#undef abort

EXPORT void abort(void) {
    raise(SIGABRT);
    hydrogen_thread_exit();
}
