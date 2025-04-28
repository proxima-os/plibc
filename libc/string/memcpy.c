#include "compiler.h"
#include "string.h"

#undef memcpy

EXPORT void *memcpy(void *s1, const void *s2, size_t n) {
    unsigned char *dest = s1;
    const unsigned char *src = s2;

    while (n--) *dest++ = *src++;

    return s1;
}
