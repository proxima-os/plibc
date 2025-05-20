#include "compiler.h"
#include "sys/stat.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <string.h>
#include <sys/types.h>

EXPORT int chmod(const char *path, mode_t mode) {
    int error = hydrogen_fs_chmod(HYDROGEN_INVALID_HANDLE, path, strlen(path), mode, 0);

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    return 0;
}
