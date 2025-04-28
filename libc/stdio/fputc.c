#include "compiler.h"
#include "stdio.h"

EXPORT int fputc(int c, FILE *stream) {
    unsigned char val = c;
    if (fwrite(&val, sizeof(val), 1, stream) != 1) return EOF;
    return val;
}
