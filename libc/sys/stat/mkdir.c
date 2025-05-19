#include "compiler.h"
#include "sys/stat.h"
#include <errno.h>
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>

EXPORT int mkdir(const char *path, mode_t mode) {
    size_t len = strlen(path);

    if (unlikely(!len)) {
        errno = ENOENT;
        return -1;
    }

    int error = hydrogen_fs_create(HYDROGEN_INVALID_HANDLE, path, len, HYDROGEN_DIRECTORY, mode);

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    return 0;
}
