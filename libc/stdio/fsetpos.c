#include "compiler.h"
#include "stdio.h"
#include <unistd.h>

EXPORT int fsetpos(FILE *stream, const fpos_t *pos) {
    if (unlikely(fflush(stream))) return -1;
    if (unlikely(lseek(stream->__fd, pos->__pos, SEEK_SET) < 0)) {
        stream->__err = 1;
        return -1;
    }

    stream->__eof = 0;
    return 0;
}
