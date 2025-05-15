#include "compiler.h"
#include "unistd.h"
#include <hydrogen/process.h>
#include <sys/types.h>

EXPORT uid_t getuid(void) {
    return hydrogen_process_getuid(HYDROGEN_THIS_PROCESS).integer;
}
