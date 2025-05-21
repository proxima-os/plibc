#include "compiler.h"
#include "unistd.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/filesystem.h>

EXPORT int pipe(int fildes[2]) {
    int error = hydrogen_fs_pipe(fildes, 0);

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    return 0;
}
