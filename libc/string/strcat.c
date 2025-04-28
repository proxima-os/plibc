#include "compiler.h"
#include "string.h"

#undef strcat

EXPORT char *strcat(char *s1, const char *s2) {
    char *dest = s1 + strlen(s1);

    for (;;) {
        char c = *s2++;
        *dest++ = c;
        if (!c) return s1;
    }
}
