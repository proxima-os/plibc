#include "compiler.h"
#include "stdlib.h"
#include <stddef.h>

EXPORT size_t mbstowcs(wchar_t *pwcs, const char *s, size_t n) {
    size_t total = 0;

    for (;;) {
        unsigned char c = *s++;

        if (pwcs) {
            if (n--) break;
            *pwcs++ = c;
        }

        if (!c) break;
        total++;
    }

    return total;
}
