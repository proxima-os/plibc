#include "compiler.h"
#include "stdio.h"
#include <errno.h>
#include <stdlib.h>

EXPORT int setvbuf(FILE *stream, char *buf, int mode, size_t size) {
    if (fflush(stream)) return 1;

    switch (mode) {
    case _IOFBF:
    case _IOLBF:
        if (!buf) {
            if (!stream->__own) {
                size = BUFSIZ;
                buf = malloc(size);
                if (!buf) return 1;
                stream->__own = 1;
            } else {
                buf = stream->__buffer;
            }
        } else {
            if (size == 0) {
                errno = EINVAL;
                return 1;
            }

            if (stream->__own) {
                free(stream->__buffer);
                stream->__own = 0;
            }
        }

        stream->__buffer = buf;
        stream->__buf_start = buf;
        stream->__buf_cur = buf;
        stream->__buf_end = buf + size;
        stream->__lbf = mode == _IOLBF;
        return 0;
    case _IONBF:
        if (stream->__own) free(stream->__buffer);
        stream->__buffer = NULL;
        stream->__buf_start = NULL;
        stream->__buf_cur = NULL;
        stream->__buf_end = NULL;
        stream->__own = 0;
        return 0;
    default: errno = EINVAL; return 1;
    }
}
