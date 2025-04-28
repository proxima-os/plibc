#include "compiler.h"
#include "stdlib.h"
#include <errno.h>
#include <stddef.h>

EXPORT size_t wcstombs(char *s, const wchar_t *pwcs, size_t n) {
    size_t total = 0;

    for (;;) {
        wchar_t c = *pwcs++;

        if (c < 0 || c > 0xff) {
            errno = EILSEQ;
            return -1;
        }

        if (s) {
            if (n--) break;
            *s++ = c;
        }

        if (!c) break;
        total++;
    }

    return total;
}
