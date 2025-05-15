#include "compiler.h"
#include "stdio.h"

EXPORT int puts(const char *s) {
    if (unlikely(fputs(s, stdout) == EOF)) return EOF;
    if (unlikely(fputc('\n', stdout) == EOF)) return EOF;
    return 1;
}
