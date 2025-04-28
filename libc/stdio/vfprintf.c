#include "compiler.h"
#include "stdio.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

static bool vfprintf_write(FILE *ctx, const void *data, size_t size) {
    return fwrite(data, size, 1, ctx) == 1;
}

#define PRINTF_WRITE vfprintf_write
#include "do_printf.c"

EXPORT int vfprintf(FILE *stream, const char *format, va_list arg) {
    return do_printf(stream, format, arg);
}
