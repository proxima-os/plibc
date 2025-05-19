#include "utime.h"
#include "compiler.h"
#include <errno.h>
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <stddef.h>
#include <string.h>

EXPORT int utime(const char *path, const struct utimbuf *times) {
    size_t length = strlen(path);

    if (unlikely(!length)) {
        errno = ENOENT;
        return -1;
    }

    int error;

    if (!times) {
        error = hydrogen_fs_utime(
                HYDROGEN_INVALID_HANDLE,
                path,
                length,
                HYDROGEN_FILE_TIME_NOW,
                HYDROGEN_FILE_TIME_NOW,
                HYDROGEN_FILE_TIME_NOW,
                0
        );
    } else {
        error = hydrogen_fs_utime(
                HYDROGEN_INVALID_HANDLE,
                path,
                length,
                times->actime * 1000000000,
                HYDROGEN_FILE_TIME_NOW,
                times->modtime * 1000000000,
                0
        );
    }

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    return 0;
}
