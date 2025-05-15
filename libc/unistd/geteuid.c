#include "compiler.h"
#include "unistd.h"
#include <hydrogen/process.h>
#include <sys/types.h>

EXPORT uid_t geteuid(void) {
    return hydrogen_process_geteuid(HYDROGEN_THIS_PROCESS).integer;
}
