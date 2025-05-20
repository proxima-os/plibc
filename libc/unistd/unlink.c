#include "compiler.h"
#include "unistd.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <stddef.h>
#include <string.h>

EXPORT int unlink(const char *path) {
    int error = hydrogen_fs_unlink(HYDROGEN_INVALID_HANDLE, path, strlen(path), 0);

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    return 0;
}
