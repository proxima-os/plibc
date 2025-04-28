#include "string.h"

#undef strncpy

char *strncpy(char *s1, const char *s2, size_t n) {
    char *d = s1;

    while (n--) {
        char c = *s2++;
        *d++ = c;

        if (!c) {
            while (n--) *d++ = 0;

            break;
        }
    }

    return d;
}
