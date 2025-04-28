#include "compiler.h"
#include "stdlib.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>

EXPORT unsigned long strtoul(const char *nptr, char **endptr, int base) {
    while (isspace(*nptr)) nptr++;

    const char *cur = nptr;

    bool negative = *cur == '-';
    if (negative || *cur == '+') cur++;

    unsigned long value = 0;

    if (base == 0) {
        if (cur[0] == '0') {
            if (cur[1] == 'x' || cur[1] == 'X') {
                base = 16;
                cur += 2;
            } else {
                base = 8;
            }
        } else {
            base = 10;
        }
    } else if (base == 16 && cur[0] == '0' && (cur[1] == 'x' || cur[1] == 'X')) {
        cur += 2;
    } else if (base < 0 || base > 36) {
        errno = EINVAL;
        goto done;
    }

    bool have_digits = false;
    bool overflow = false;

    for (;;) {
        char c = *cur;
        int dval;

        if (c >= '0' && c <= '9') dval = c - '0';
        else if (c >= 'a' && c <= 'z') dval = c - 'a' + 10;
        else if (c >= 'A' && c <= 'Z') dval = c - 'A' + 10;
        else break;

        if (dval >= base) break;

        unsigned long nval = (value * 10) + dval;
        if (nval < value) overflow = true;
        value = nval;
        have_digits = true;

        cur++;
    }

    if (!have_digits) goto done;
    nptr = cur;

    if (overflow) {
        errno = ERANGE;
        value = ULONG_MAX;
    } else if (negative) {
        value = -value;
    }

done:
    if (endptr) *endptr = (char *)nptr;
    return value;
}
