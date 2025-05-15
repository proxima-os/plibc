#include "compiler.h"
#include "stdio.h"
#include "stdio.p.h"
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

EXPORT FILE *fopen(const char *filename, const char *mode) {
    int flags = get_open_flags(mode);
    if (unlikely(flags < 0)) return NULL;

    FILE *stream = malloc(sizeof(*stream));
    if (unlikely(!stream)) return NULL;

    int fd = open(filename, flags, FOPEN_MODE);
    if (unlikely(fd < 0)) {
        free(stream);
        return NULL;
    }

    if (unlikely(do_open(stream, fd, flags))) {
        int orig_errno = errno;
        close(fd);
        free(stream);
        errno = orig_errno;
        return NULL;
    }

    return stream;
}
