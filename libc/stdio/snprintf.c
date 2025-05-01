#include "compiler.h"
#include "stdio.h"
#include "stdio.p.h" /* IWYU pragma: keep */
#include <stdarg.h>

EXPORT int snprintf(char *s, size_t n, const char *restrict format, ...) {
    va_list arg;
    va_start(arg, format);
    int ret = vsnprintf(s, n, format, arg);
    va_end(arg);
    return ret;
}
