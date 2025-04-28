#include "compiler.h"
#include "stdio.h"
#include "stdio.p.h" /* IWYU pragma: keep */
#include <stdarg.h>

EXPORT int fscanf(FILE *stream, const char *format, ...) {
    va_list arg;
    va_start(arg, format);
    int ret = vfscanf(stream, format, arg);
    va_end(arg);
    return ret;
}
