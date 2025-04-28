#include "compiler.h"
#include "stdio.h"
#include <string.h>

EXPORT int fputs(const char *s, FILE *stream) {
    size_t len = strlen(s);
    if (fwrite(s, sizeof(*s), len, stream) != len) return EOF;
    return 1;
}
