#include "compiler.h"
#include "string.h"
#include <stddef.h>

#undef memset

EXPORT void *memset(void *s, int c, size_t n) {
    unsigned char *dest = s;
    unsigned char fill = c;

    while (n--) *dest++ = fill;

    return s;
}
