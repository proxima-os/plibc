#include "compiler.h"
#include "stdio.h"
#include "stdio.p.h" /* IWYU pragma: keep */
#include <stdarg.h>
#include <stdint.h>

EXPORT int vsprintf(char *s, const char *format, va_list arg) {
    return vsnprintf(s, SIZE_MAX, format, arg);
}
