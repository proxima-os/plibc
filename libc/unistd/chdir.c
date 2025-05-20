#include "compiler.h"
#include "unistd.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <hydrogen/process.h>
#include <string.h>

EXPORT int chdir(const char *path) {
    int error = hydrogen_fs_chdir(HYDROGEN_THIS_PROCESS, HYDROGEN_INVALID_HANDLE, path, strlen(path));

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    return error;
}
