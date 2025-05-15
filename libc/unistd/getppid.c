#include "compiler.h"
#include "unistd.h"
#include <hydrogen/process.h>
#include <sys/types.h>

EXPORT pid_t getppid(void) {
    return hydrogen_process_getppid(HYDROGEN_THIS_PROCESS).integer;
}
