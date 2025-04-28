#include "compiler.h"
#include "string.h"

#undef strncmp

EXPORT int strncmp(const char *s1, const char *s2, size_t n) {
    while (n--) {
        unsigned char c1 = *s1++;
        unsigned char c2 = *s2++;

        if (c1 < c2) return -1;
        if (c1 > c2) return 1;
        if (c1 == 0) break;
    }

    return 0;
}
