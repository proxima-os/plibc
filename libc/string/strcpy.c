#include "compiler.h"
#include "string.h"

#undef strcpy

EXPORT char *strcpy(char *s1, const char *s2) {
    char *dest = s1;

    for (;;) {
        char c = *s2++;
        *dest++ = c;
        if (!c) return s1;
    }
}
