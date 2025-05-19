#include "compiler.h"
#include "dirent.h"
#include <errno.h> /* IWYU pragma: keep */
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

#define INIT_BUFFER_SIZE 1024

EXPORT DIR *opendir(const char *dirname) {
    int fd = open(dirname, O_RDONLY | __O_DIRECTORY | __O_CLOEXEC);
    if (unlikely(fd < 0)) return NULL;

    DIR *dir = calloc(1, sizeof(*dir));
    if (unlikely(dir)) {
        int orig_errno = errno;
        close(fd);
        errno = orig_errno;
        return NULL;
    }

    dir->__buf_head = malloc(INIT_BUFFER_SIZE);
    if (unlikely(!dir->__buf_head)) {
        close(fd);
        errno = ENOMEM;
        return NULL;
    }
    dir->__buf_cur = dir->__buf_head;
    dir->__buf_end = dir->__buf_head;
    dir->__buf_tail = dir->__buf_head + INIT_BUFFER_SIZE;

    dir->__fd = fd;
    return dir;
}
