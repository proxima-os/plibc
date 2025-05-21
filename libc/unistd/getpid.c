#include "compiler.h"
#include "unistd.h"
#include "unistd.p.h"
#include <hydrogen/process.h>
#include <sys/types.h>

EXPORT pid_t getpid(void) {
    pid_t pid = __atomic_load_n(&cur_pid, __ATOMIC_RELAXED);

    if (pid < 0) {
        pid = hydrogen_process_getpid(HYDROGEN_THIS_PROCESS).integer;
        __atomic_store_n(&cur_pid, pid, __ATOMIC_RELAXED);
    }

    return pid;
}
