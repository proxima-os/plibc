#include "sys/wait.h"
#include "compiler.h"
#include "stub.h"
#include <sys/types.h>

EXPORT pid_t wait(int *stat_loc) {
    return waitpid(-1, stat_loc, 0);
}

EXPORT pid_t waitpid(pid_t pid, int *stat_loc, int options) {
    STUB();
    return -1;
}
