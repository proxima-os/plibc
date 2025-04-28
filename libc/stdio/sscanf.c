#include "stdio.h"
#include "compiler.h"
#include "stdio.p.h" /* IWYU pragma: keep */
#include <stdarg.h>

EXPORT int sscanf(const char *s, const char *format, ...) {
    va_list arg;
    va_start(arg, format);
    int ret = vsscanf(s, format, arg);
    va_end(arg);
    return ret;
}
