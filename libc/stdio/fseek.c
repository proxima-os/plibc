#include "compiler.h"
#include "stdio.h"
#include <unistd.h>

EXPORT int fseek(FILE *stream, long offset, int whence) {
    if (fflush(stream)) return -1;
    if (lseek(stream->__fd, offset, whence) < 0) {
        stream->__err = 1;
        return -1;
    }

    stream->__eof = 0;
    return 0;
}
