#include "compiler.h"
#include "unistd.h"
#include <errno.h> /* IWYU pragma: keep */
#include <fcntl.h>
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <stddef.h>
#include <string.h>

EXPORT int rmdir(const char *path) {
    int error = hydrogen_fs_unlink(HYDROGEN_INVALID_HANDLE, path, strlen(path), __AT_REMOVEDIR);

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    return 0;
}
