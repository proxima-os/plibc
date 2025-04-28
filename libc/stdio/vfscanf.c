#include "compiler.h"
#include "stdio.h"
#include "stdio.p.h" /* IWYU pragma: keep */

static int vfscanf_get(FILE *stream) {
    int value = fgetc(stream);
    if (value != EOF) return value;
    if (feof(stream)) return SCANF_EOF;
    return SCANF_ERR;
}

#define SCANF_GET vfscanf_get
#define SCANF_PUT ungetc
#include "do_scanf.c"

EXPORT int vfscanf(FILE *stream, const char *format, va_list arg) {
    return do_scanf(stream, format, arg);
}
