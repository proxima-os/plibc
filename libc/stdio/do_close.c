#include "stdio.p.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

static int close_fd(int fd) {
    if (fd_valid(fd)) {
        fd_bitmap &= ~(1 << fd);
        return 0;
    }

    errno = EBADF;
    return -1;
}

int do_close(FILE *stream) {
    int err = fflush(stream);
    int orig_errno = errno;

    if (stream->__own) {
        free(stream->__buffer);
        stream->__buffer = NULL;
        stream->__buf_start = NULL;
        stream->__buf_cur = NULL;
        stream->__buf_end = NULL;
        stream->__own = 0;
    }

    if (close_fd(stream->__fd) && !err) return EOF;
    if (err) errno = orig_errno;
    return err;
}
