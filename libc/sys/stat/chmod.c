#include "compiler.h"
#include "sys/stat.h"
#include <errno.h>
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <string.h>
#include <sys/types.h>

EXPORT int chmod(const char *path, mode_t mode) {
    size_t len = strlen(path);

    if (unlikely(len == 0)) {
        errno = ENOENT;
        return -1;
    }

    int error = hydrogen_fs_chmod(HYDROGEN_INVALID_HANDLE, path, len, mode, 0);

    if (unlikely(error)) {
        errno = ENOENT;
        return -1;
    }

    return 0;
}
