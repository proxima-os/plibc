#include "unistd.h"
#include "compiler.h"
#include <hydrogen/process.h>
#include <sys/types.h>

EXPORT gid_t getegid(void) {
    return hydrogen_process_getegid(HYDROGEN_THIS_PROCESS).integer;
}
