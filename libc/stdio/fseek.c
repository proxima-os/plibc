#include "compiler.h"
#include "stdio.h"
#include <unistd.h>

EXPORT int fseek(FILE *stream, long offset, int whence) {
    if (unlikely(fflush(stream))) return -1;
    if (unlikely(lseek(stream->__fd, offset, whence)) < 0) {
        stream->__err = 1;
        return -1;
    }

    stream->__eof = 0;
    return 0;
}
