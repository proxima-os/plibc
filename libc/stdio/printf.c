#include "compiler.h"
#include "stdio.h"
#include <stdarg.h>

EXPORT int printf(const char *format, ...) {
    va_list arg;
    va_start(arg, format);
    int ret = vfprintf(stdout, format, arg);
    va_end(arg);
    return ret;
}
