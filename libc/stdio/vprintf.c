#include "compiler.h"
#include "stdio.h"
#include <stdarg.h>

EXPORT int vprintf(const char *format, va_list arg) {
    return vfprintf(stdout, format, arg);
}
