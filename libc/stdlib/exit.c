#include "compiler.h"
#include "cxa.h"
#include "rtld.h"
#include "stdlib.h"
#include <hydrogen/thread.h>
#include <stdio.h>

EXPORT void exit(int status) {
    __cxa_finalize(NULL);
    __plibc_rtld_run_fini();
    fflush(NULL);
    hydrogen_thread_exit();
}
