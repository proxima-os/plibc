#include "compiler.h"
#include "string.h"
#include <stddef.h>

EXPORT size_t strxfrm(char *s1, const char *s2, size_t n) {
    size_t len = 0;

    for (;;) {
        char c = *s2++;
        if (n--) *s1++ = c;
        if (!c) break;
        len++;
    }

    return len;
}
