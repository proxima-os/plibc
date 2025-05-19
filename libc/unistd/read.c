#include "compiler.h"
#include "unistd.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/filesystem.h>
#include <hydrogen/types.h>
#include <limits.h>
#include <sys/types.h>

EXPORT ssize_t read(int fildes, void *buf, size_t nbyte) {
    if (nbyte > SSIZE_MAX) nbyte = SSIZE_MAX;

    hydrogen_ret_t ret = hydrogen_fs_read(fildes, buf, nbyte);

    if (unlikely(ret.error)) {
        errno = ret.error;
        return -1;
    }

    return ret.integer;
}
