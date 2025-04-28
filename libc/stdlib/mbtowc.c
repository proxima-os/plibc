#include "compiler.h"
#include "stdlib.h"
#include <errno.h>

EXPORT int mbtowc(wchar_t *pwc, const char *s, size_t n) {
    if (!s) return 0;
    if (!n) {
        errno = EILSEQ;
        return -1;
    }

    if (pwc) *pwc = (unsigned char)*s;
    return *s ? 1 : 0;
}
