#include "stdio.h"
#include "compiler.h"
#include <stdarg.h>

EXPORT int fprintf(FILE *stream, const char *format, ...) {
    va_list arg;
    va_start(arg, format);
    int ret = vfprintf(stream, format, arg);
    va_end(arg);
    return ret;
}

