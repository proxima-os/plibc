#include "compiler.h"
#include "stdio.h"
#include "stdio.p.h"
#include <errno.h> /* IWYU pragma: keep */
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

EXPORT FILE *freopen(const char *filename, const char *mode, FILE *stream) {
    fflush(stream);

    int flags = get_open_flags(mode);
    if (unlikely(flags < 0)) goto close_cleanup;

    if (filename) {
        do_close(stream, true);

        int fd = open(filename, flags, FOPEN_MODE);
        if (unlikely(fd < 0)) goto cleanup;

        if (unlikely(do_open(stream, fd, flags))) {
            stream->__fd = fd;
            goto close_cleanup;
        }

        return stream;
    }

    if ((flags & O_RDONLY) && !stream->__fd_read) goto no_reuse_fd;
    if ((flags & O_WRONLY) && !stream->__fd_write) goto no_reuse_fd;

    if (unlikely(fcntl(stream->__fd, F_SETFL, flags) == -1)) {
        int orig_errno = errno;
        do_close(stream, true);
        errno = orig_errno;
        goto cleanup;
    }

    stream->__eof = 0;
    stream->__err = 0;
    return stream;
no_reuse_fd:
    errno = EBADF;
close_cleanup: {
    int orig_errno = errno;
    close(stream->__fd);
    errno = orig_errno;
}
cleanup:
    free(stream);
    return NULL;
}
