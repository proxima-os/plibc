#include "compiler.h"
#include "unistd.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/process.h>
#include <sys/types.h>

EXPORT int setgid(gid_t gid) {
    int error = hydrogen_process_setgid(HYDROGEN_THIS_PROCESS, gid);

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    return 0;
}
