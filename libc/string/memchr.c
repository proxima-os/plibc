#include "compiler.h"
#include "string.h"
#include <stddef.h>

#undef memchr

EXPORT void *memchr(const void *s, int c, size_t n) {
    const unsigned char *area = s;
    unsigned char find = c;

    while (n--) {
        if (*area == find) return (void *)area;
        area++;
    }

    return NULL;
}
