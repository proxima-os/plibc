#include "compiler.h"
#include "string.h"

EXPORT char *strtok(char *s1, const char *s2) {
    static char *next_token;
    if (s1) next_token = s1;

    for (;;) {
        char c = *next_token;
        if (!c) return NULL;
        if (!strchr(s2, c)) break;
        next_token++;
    }

    char *start = next_token;

    for (;;) {
        char c = *next_token;
        if (!c) break;
        next_token++;

        if (strchr(s2, c)) {
            next_token[-1] = 0;
            break;
        }
    }

    return start;
}
