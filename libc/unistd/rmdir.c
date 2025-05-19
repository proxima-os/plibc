#include "compiler.h"
#include "unistd.h"
#include <errno.h>
#include <fcntl.h>
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <stddef.h>
#include <string.h>

EXPORT int rmdir(const char *path) {
    size_t len = strlen(path);

    if (unlikely(!len)) {
        errno = ENOENT;
        return -1;
    }

    int error = hydrogen_fs_unlink(HYDROGEN_INVALID_HANDLE, path, len, __AT_REMOVEDIR);

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    return 0;
}
