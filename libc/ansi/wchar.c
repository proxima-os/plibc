#include "wchar.h"
#include "compiler.h"
#include <stdio.h>
#include <stdlib.h>

EXPORT wint_t fputwc(wchar_t c, FILE *stream) {
    char buf[MB_CUR_MAX];
    int len = wctomb(buf, c);
    if (len < 0) return WEOF;

    if (fwrite(buf, sizeof(*buf), len, stream) != (size_t)len) return -1;

    return c;
}

EXPORT wint_t putwc(wchar_t c, FILE *stream) {
    return fputwc(c, stream);
}
