#include "compiler.h"
#include <stdio.h>

EXPORT int fgetc(FILE *stream) {
    unsigned char c;
    if (unlikely(fread(&c, sizeof(c), 1, stream) != 1)) return EOF;
    return c;
}
