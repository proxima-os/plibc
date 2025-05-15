#include "compiler.h"
#include "unistd.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/process.h>
#include <sys/types.h>

EXPORT int setuid(uid_t uid) {
    int error = hydrogen_process_setuid(HYDROGEN_THIS_PROCESS, uid);

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    return 0;
}
