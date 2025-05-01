#include "compiler.h"
#include "stdio.h"
#include <sys/types.h>
#include <unistd.h>

EXPORT int fgetpos(FILE *stream, fpos_t *pos) {
    off_t off = lseek(stream->__fd, 0, SEEK_CUR);
    if (off < 0) return -1;

    if (stream->__buffer) {
        if (stream->__rbf) {
            off -= stream->__buf_cur - stream->__buffer;
        } else {
            off += stream->__buf_cur - stream->__buf_start;
        }
    }

    pos->__pos = off;
    return 0;
}
