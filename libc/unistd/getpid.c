#include "compiler.h"
#include "unistd.h"
#include <hydrogen/process.h>
#include <sys/types.h>

EXPORT pid_t getpid(void) {
    return hydrogen_process_getpid(HYDROGEN_THIS_PROCESS).integer;
}
