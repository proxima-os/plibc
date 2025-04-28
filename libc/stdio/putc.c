#include "compiler.h"
#include "stdio.h"

EXPORT int putc(int c, FILE *stream) {
    return fputc(c, stream);
}
