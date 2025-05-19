#include "compiler.h"
#include "unistd.h"
#include <errno.h>
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <hydrogen/process.h>
#include <string.h>

EXPORT int chdir(const char *path) {
    size_t length = strlen(path);

    if (unlikely(!length)) {
        errno = ENOENT;
        return -1;
    }

    int error = hydrogen_fs_chdir(HYDROGEN_THIS_PROCESS, HYDROGEN_INVALID_HANDLE, path, length);

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    return error;
}
