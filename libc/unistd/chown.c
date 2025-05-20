#include "compiler.h"
#include "unistd.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>

EXPORT int chown(const char *path, uid_t owner, gid_t group) {
    int error = hydrogen_fs_chown(HYDROGEN_INVALID_HANDLE, path, strlen(path), owner, group, 0);

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    return 0;
}
