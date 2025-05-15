#include "compiler.h"
#include "stdio.h"

EXPORT int ungetc(int c, FILE *stream) {
    if (c != EOF) {
        if (unlikely(stream->__nunget == 4)) return EOF;
        stream->__unget <<= 8;
        stream->__unget |= (unsigned char)c;
        stream->__nunget += 1;
        stream->__eof = 0;
    }

    return c;
}
