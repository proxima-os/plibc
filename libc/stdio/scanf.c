#include "compiler.h"
#include "stdio.h"
#include "stdio.p.h" /* IWYU pragma: keep */
#include <stdarg.h>

EXPORT int scanf(const char *format, ...) {
    va_list arg;
    va_start(arg, format);
    int ret = vfscanf(stdin, format, arg);
    va_end(arg);
    return ret;
}
