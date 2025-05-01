#include "compiler.h"
#include "sys/types.h"
#include "unistd.h"
#include "unistd.p.h"
#include <errno.h>
#include <hydrogen/log.h>
#include <limits.h>

EXPORT ssize_t write(int fildes, const void *buf, size_t nbyte) {
    // Stub
    if (!nbyte) return 0;

    if (!fd_valid(fildes)) {
        errno = EBADF;
        return -1;
    }

    if (nbyte > SSIZE_MAX) nbyte = SSIZE_MAX;

    int error = hydrogen_log_write(log_handle, buf, nbyte);
    if (error) {
        errno = error;
        return -1;
    }

    return nbyte;
}
