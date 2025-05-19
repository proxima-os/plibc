#include "compiler.h"
#include "unistd.h"
#include <errno.h>
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <stdint.h>
#include <string.h>

EXPORT int access(const char *path, int amode) {
    size_t length = strlen(path);

    if (unlikely(!length)) {
        errno = ENOENT;
        return -1;
    }

    uint32_t type = 0;

    if (amode & R_OK) type |= HYDROGEN_FILE_READ;
    if (amode & W_OK) type |= HYDROGEN_FILE_WRITE;
    if (amode & X_OK) type |= HYDROGEN_FILE_EXEC;

    int error = hydrogen_fs_access(HYDROGEN_INVALID_HANDLE, path, length, type, 0);

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    return 0;
}
