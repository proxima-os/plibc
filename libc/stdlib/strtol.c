#include "compiler.h"
#include "stdlib.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>

EXPORT long strtol(const char *nptr, char **endptr, int base) {
    while (isspace(*nptr)) nptr++;

    const char *cur = nptr;

    bool negative = *cur == '-';
    if (negative || *cur == '+') cur++;

    long value = 0;

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

    unsigned long uval = 0;
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

        unsigned long nval = (uval * 10) + dval;
        if (nval < uval) overflow = true;
        uval = nval;
        have_digits = true;

        cur++;
    }

    if (!have_digits) goto done;
    nptr = cur;

    if (negative) {
        if (overflow || uval > (unsigned long)LONG_MAX + 1) {
            errno = ERANGE;
            value = LONG_MIN;
        } else {
            value = -(long)uval;
        }
    } else if (overflow || uval > LONG_MAX) {
        errno = ERANGE;
        value = LONG_MAX;
    } else {
        value = uval;
    }

done:
    if (endptr) *endptr = (char *)nptr;
    return value;
}
