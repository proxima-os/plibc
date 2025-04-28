#include "compiler.h"
#include "string.h"
#include <stdint.h>

#undef memmove

EXPORT void *memmove(void *s1, const void *s2, size_t n) {
    unsigned char *dest = s1;
    const unsigned char *src = s2;

    if ((uintptr_t)s1 < (uintptr_t)s2) {
        while (n--) *dest++ = *src++;
    } else if ((uintptr_t)s1 > (uintptr_t)s2) {
        dest += n;
        src += n;

        while (n--) *--dest = *--src;
    }

    return s1;
}
