#include "compiler.h"
#include "stdlib.h"
#include <hydrogen/process.h>
#include <signal.h>

#undef abort

EXPORT void abort(void) {
    raise(SIGABRT);
    hydrogen_process_exit(127);
}
