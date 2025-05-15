#include "compiler.h"
#include "unistd.h"
#include <hydrogen/process.h>
#include <sys/types.h>

EXPORT gid_t getgid(void) {
    return hydrogen_process_getgid(HYDROGEN_THIS_PROCESS).integer;
}
