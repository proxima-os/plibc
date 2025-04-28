#include "compiler.h"
#include "stdlib.h"
#include <errno.h>

EXPORT int wctomb(char *s, wchar_t wchar) {
    if (!s) return 0;
    if (wchar < 0 || wchar > 0xff) {
        errno = EILSEQ;
        return -1;
    }

    *s = wchar;
    return 1;
}
