#include "compiler.h"
#include "string.h"

#undef strchr

EXPORT char *strchr(const char *s, int c) {
    char find = c;

    for (;;) {
        char cur = *s;
        if (cur == find) return (char *)s;
        if (!cur) return NULL;
        s++;
    }
}
