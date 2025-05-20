#include "compiler.h"
#include "sys/stat.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>

EXPORT int mkdir(const char *path, mode_t mode) {
    int error = hydrogen_fs_create(HYDROGEN_INVALID_HANDLE, path, strlen(path), HYDROGEN_DIRECTORY, mode);

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    return 0;
}
