#include "compiler.h"
#include "fcntl.h"
#include <errno.h>
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <hydrogen/types.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>

EXPORT int open(const char *path, int oflag, ...) {
    if (unlikely((oflag & O_ACCMODE) == 0)) {
        errno = EINVAL;
        return -1;
    }

    uint32_t mode;

    if (oflag & O_CREAT) {
        va_list args;
        va_start(args, oflag);
        mode = va_arg(args, mode_t) & 0777;
        va_end(args);
    } else {
        mode = 0;
    }

    hydrogen_ret_t ret = hydrogen_fs_open(HYDROGEN_INVALID_HANDLE, path, strlen(path), oflag, mode);

    if (unlikely(ret.error)) {
        errno = ret.error;
        return -1;
    }

    return ret.integer;
}
