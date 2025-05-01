#include "sys/wait.h"
#include "compiler.h"

EXPORT int wait(int *stat_loc) {
    return waitpid(-1, stat_loc, 0);
}
