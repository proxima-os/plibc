#include "compiler.h"
#include "sys/types.h"
#include "unistd.h"
#include <errno.h>

EXPORT ssize_t write(int fildes, const void *buf, size_t nbyte) {
    // Stub
    errno = EBADF;
    return -1;
}
