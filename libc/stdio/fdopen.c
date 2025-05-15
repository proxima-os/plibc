#include "compiler.h"
#include "stdio.h"
#include "stdio.p.h"
#include <fcntl.h>
#include <stdlib.h>

EXPORT FILE *fdopen(int fildes, const char *type) {
    int flags = get_open_flags(type);
    if (unlikely(flags < 0)) return NULL;

    FILE *file = malloc(sizeof(*file));
    if (unlikely(!file)) return NULL;

    if (unlikely(do_open(file, fildes, flags))) {
        free(file);
        return NULL;
    }

    /*if (flags & O_CLOEXEC) {
        int fdfl = fcntl(fildes, F_GETFD);
        if (fdfl == -1 || (!(fdfl & FD_CLOEXEC) && fcntl(fildes, F_SETFD, fdfl | FD_CLOEXEC) == -1)) {
            int orig_errno = errno;
            do_close(file, false);
            errno = orig_errno;
            return NULL;
        }
    }*/

    return file;
}
