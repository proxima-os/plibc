#include "compiler.h"
#include "stdio.h"

EXPORT char *fgets(char *s, int n, FILE *stream) {
    char *start = s;

    for (int i = 1; i < n; i++) {
        int value = fgetc(stream);

        if (value == EOF) {
            if (i == 1 || unlikely(!feof(stream))) return NULL;
            break;
        }

        *s++ = (unsigned char)value;
        if (value == '\n') break;
    }

    if (n) *s = 0;
    return start;
}
