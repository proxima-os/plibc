#include "compiler.h"
#include "stdio.h"

EXPORT long ftell(FILE *stream) {
    fpos_t pos;
    if (fgetpos(stream, &pos)) return -1;

    pos.__pos -= stream->__nunget;
    if (pos.__pos < 0) pos.__pos = 0;
    return pos.__pos;
}
