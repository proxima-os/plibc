#include "compiler.h"
#include "stdio.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <string.h>

EXPORT int rename(const char *old, const char *new) {
    int error = hydrogen_fs_rename(
            HYDROGEN_INVALID_HANDLE,
            old,
            strlen(old),
            HYDROGEN_INVALID_HANDLE,
            new,
            strlen(new)
    );

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    return 0;
}
