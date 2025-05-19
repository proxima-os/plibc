#include "compiler.h"
#include "sys/types.h"
#include "unistd.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/types.h>
#include <hydrogen/filesystem.h>
#include <limits.h>

EXPORT ssize_t write(int fildes, const void *buf, size_t nbyte) {
    if (nbyte > SSIZE_MAX) nbyte = SSIZE_MAX;

    hydrogen_ret_t ret = hydrogen_fs_write(fildes, buf, nbyte);
    if (unlikely(ret.error)) {
        errno = ret.error;
        return -1;
    }

    return ret.integer;
}
