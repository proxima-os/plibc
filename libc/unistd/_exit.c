#include "compiler.h"
#include "unistd.h"
#include <hydrogen/thread.h>

EXPORT void _exit(int status) {
    hydrogen_thread_exit();
}
