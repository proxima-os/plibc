#include "compiler.h"
#include "unistd.h"
#include <hydrogen/process.h>

EXPORT void _exit(int status) {
    hydrogen_process_exit(status);
}
