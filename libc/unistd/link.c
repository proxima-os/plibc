#include "compiler.h"
#include "unistd.h"
#include <errno.h>
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <string.h>

EXPORT int link(const char *existing, const char *new) {
    size_t existing_len = strlen(existing);
    size_t new_len = strlen(new);

    if (unlikely(!existing) || unlikely(!new_len)) {
        errno = ENOENT;
        return -1;
    }

    int error = hydrogen_fs_link(
            HYDROGEN_INVALID_HANDLE,
            new,
            new_len,
            HYDROGEN_INVALID_HANDLE,
            existing,
            existing_len,
            0
    );

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    return 0;
}
