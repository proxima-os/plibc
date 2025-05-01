#include "compiler.h"
#include "stdio.h"
#include <unistd.h>

EXPORT int fsetpos(FILE *stream, const fpos_t *pos) {
    if (fflush(stream)) return -1;
    if (lseek(stream->__fd, pos->__pos, SEEK_SET) < 0) {
        stream->__err = 1;
        return -1;
    }

    stream->__eof = 0;
    return 0;
}
