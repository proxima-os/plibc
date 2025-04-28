#include "compiler.h"
#include "stdio.h"
#include <stdarg.h>

EXPORT int sprintf(char *s, const char *format, ...) {
    va_list arg;
    va_start(arg, format);
    int ret = vsprintf(s, format, arg);
    va_end(arg);
    return ret;
}
