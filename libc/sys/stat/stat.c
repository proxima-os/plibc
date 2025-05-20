#include "sys/stat.h"
#include "compiler.h"
#include "sys/stat.p.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <string.h>

EXPORT int stat(const char *path, struct stat *buf) {
    hydrogen_file_information_t info;
    int error = hydrogen_fs_stat(HYDROGEN_INVALID_HANDLE, path, strlen(path), &info, 0);

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    if (unlikely(convert_stat(buf, &info))) return -1;

    return 0;
}
