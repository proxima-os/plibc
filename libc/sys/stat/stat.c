#include "sys/stat.h"
#include "compiler.h"
#include "sys/stat.p.h"
#include <errno.h>
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <string.h>

EXPORT int stat(const char *path, struct stat *buf) {
    size_t len = strlen(path);

    if (unlikely(!len)) {
        errno = ENOENT;
        return -1;
    }

    hydrogen_file_information_t info;
    int error = hydrogen_fs_stat(HYDROGEN_INVALID_HANDLE, path, len, &info, 0);

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    if (unlikely(convert_stat(buf, &info))) return -1;

    return 0;
}
