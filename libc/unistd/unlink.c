#include "compiler.h"
#include "unistd.h"
#include <errno.h>
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <stddef.h>
#include <string.h>

EXPORT int unlink(const char *path) {
    size_t length = strlen(path);

    if (unlikely(!length)) {
        errno = ENOENT;
        return -1;
    }

    int error = hydrogen_fs_unlink(HYDROGEN_INVALID_HANDLE, path, length, 0);

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    return 0;
}
