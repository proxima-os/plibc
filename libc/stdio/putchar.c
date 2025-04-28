#include "compiler.h"
#include "stdio.h"

EXPORT int putchar(int c) {
    return fputc(c, stdout);
}
