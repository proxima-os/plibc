#include "compiler.h"
#include "string.h"

EXPORT char *strstr(const char *s1, const char *s2) {
    for (;;) {
        const char *start = s1;
        const char *match = s2;

        for (;;) {
            char c2 = *match++;
            if (!c2) return (char *)s1;

            char c1 = *start++;
            if (!c1) return NULL;
            if (c1 != c2) break;
        }

        s1++;
    }
}
