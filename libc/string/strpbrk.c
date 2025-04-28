#include "compiler.h"
#include "string.h"

EXPORT char *strpbrk(const char *s1, const char *s2) {
    for (;;) {
        char c = *s1;
        if (!c) return NULL;

        const char *chars = s2;

        for (;;) {
            char cur = *chars++;
            if (cur == c) return (char *)s1;
        }

        s1++;
    }
}
