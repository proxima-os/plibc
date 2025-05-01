#include "stdio.p.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int do_open(FILE *stream, int fd, int flags) {
    memset(stream, 0, sizeof(*stream));
    stream->__fd = fd;
    stream->__fd_read = !!(flags & O_RDONLY);
    stream->__fd_write = !!(flags & O_WRONLY);

    setvbuf(stream, NULL, isatty(fd) ? _IOLBF : _IOFBF, 0);
    return 0;
}
