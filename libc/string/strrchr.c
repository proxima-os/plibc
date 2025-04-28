#include "compiler.h"
#include "string.h"
#include <stddef.h>

EXPORT char *strrchr(const char *s, int c) {
    char find = c;
    char *last = NULL;

    for (;;) {
        char cur = *s;
        if (cur == find) last = (char *)s;
        if (!cur) break;
        s++;
    }

    return last;
}
