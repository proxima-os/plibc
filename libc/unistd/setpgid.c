#include "compiler.h"
#include "unistd.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/process.h>
#include <sys/types.h>

EXPORT int setpgid(pid_t pid, pid_t pgid) {
    int error = hydrogen_process_setpgid(HYDROGEN_THIS_PROCESS, pgid);

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    return 0;
}
