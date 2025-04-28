#include "compiler.h"
#include "string.h"

#undef strncat

EXPORT char *strncat(char *s1, const char *s2, size_t n) {
    char *dest = s1 + strlen(s1);

    while (n--) {
        char c = *s2++;
        if (!c) break;
        *dest++ = c;
    }

    *dest = 0;
    return s1;
}
