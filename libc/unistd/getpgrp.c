#include "compiler.h"
#include "unistd.h"
#include <hydrogen/process.h>
#include <sys/types.h>

EXPORT pid_t getpgrp(void) {
    return hydrogen_process_getpgid(HYDROGEN_THIS_PROCESS).integer;
}
