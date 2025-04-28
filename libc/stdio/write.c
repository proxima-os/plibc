#include "stdint.p.h"
#include "stdio.p.h"
#include <errno.h>
#include <hydrogen/log.h>

ssize_t write(int fd, const void *buf, ssize_t count) {
    if (!fd_valid(fd)) {
        errno = EBADF;
        return -1;
    }

    int error = hydrogen_log_write(log_handle, buf, count);
    if (!error) return count;

    errno = error;
    return -1;
}
