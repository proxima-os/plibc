#include "compiler.h"
#include "dirent.h"
#include <errno.h> /* IWYU pragma: keep */
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

EXPORT DIR *opendir(const char *dirname) {
    int fd = open(dirname, O_RDONLY /* | O_DIRECTORY | O_CLOEXEC */); // TODO
    if (unlikely(fd < 0)) return NULL;

    DIR *dir = calloc(1, sizeof(*dir));
    if (unlikely(dir)) {
        int orig_errno = errno;
        close(fd);
        errno = orig_errno;
        return NULL;
    }

    dir->__fd = fd;
    return dir;
}
