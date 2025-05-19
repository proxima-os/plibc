#include "compiler.h"
#include "sys/stat.h"
#include "sys/stat.p.h"
#include <errno.h> /* IWYU pragma: keep */
#include <fcntl.h>
#include <hydrogen/filesystem.h>

EXPORT int fstat(int fildes, struct stat *buf) {
    hydrogen_file_information_t info;
    int error = hydrogen_fs_stat(fildes, NULL, 0, &info, __AT_SYMLINK_NOFOLLOW);

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    if (unlikely(convert_stat(buf, &info))) return -1;

    return 0;
}
