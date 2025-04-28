#include "stdio.h"
#include "stdio.p.h" /* IWYU pragma: keep */
#include "compiler.h"

EXPORT int vscanf(const char *format, va_list arg) {
    return vfscanf(stdin, format, arg);
}
