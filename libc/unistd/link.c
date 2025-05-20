#include "compiler.h"
#include "unistd.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <string.h>

EXPORT int link(const char *existing, const char *new) {
    int error = hydrogen_fs_link(
            HYDROGEN_INVALID_HANDLE,
            new,
            strlen(new),
            HYDROGEN_INVALID_HANDLE,
            existing,
            strlen(existing),
            0
    );

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    return 0;
}
