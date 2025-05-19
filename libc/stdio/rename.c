#include "compiler.h"
#include "stdio.h"
#include <errno.h>
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <string.h>

EXPORT int rename(const char *old, const char *new) {
    size_t old_len = strlen(old);
    size_t new_len = strlen(new);

    if (unlikely(!old_len) || unlikely(!new_len)) {
        errno = ENOENT;
        return -1;
    }

    int error = hydrogen_fs_rename(HYDROGEN_INVALID_HANDLE, old, old_len, HYDROGEN_INVALID_HANDLE, new, new_len);

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    return 0;
}
