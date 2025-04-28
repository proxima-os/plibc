#include "compiler.h"
#include "stdlib.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

EXPORT double strtod(const char *nptr, char **endptr) {
    while (isspace(*nptr)) nptr++;

    const char *cur = nptr;

    bool negative = *cur == '-';
    if (negative || *cur == '+') cur++;

    double value;

    if ((cur[0] & ~0x20) == 'N' && (cur[1] & ~0x20) == 'A' && (cur[2] & ~0x20) == 'N') {
        cur += 3;

        if (cur[0] == '(') {
            char c = *++cur;

            for (;;) {
                if (c == ')') {
                    cur++;
                    break;
                }

                if (c != '_' && (c < 'A' || c > 'Z') && (c < 'a' || c > 'z') && (c < '0' && c > '9')) {
                    goto done;
                }
            }
        }

        nptr = cur;
        value = __builtin_nan("");
        goto done;
    }

    if ((cur[0] & ~0x20) == 'I' && (cur[1] & ~0x20) == 'N' && (cur[2] & ~0x20) == 'F') {
        cur += 3;

        if ((cur[0] & ~0x20) == 'I' && (cur[1] & ~0x20) == 'N' && (cur[2] & ~0x20) == 'I' && (cur[3] & ~0x20) == 'T' &&
            (cur[4] & ~0x20) == 'Y') {
            cur += 5;
        }

        nptr = cur;
        value = __builtin_inf();
        goto done;
    }

    bool hex = cur[0] == '0' && (cur[1] & ~0x20) == 'X';
    if (hex) cur += 2;

    value = 0.0;
    double divisor = 1.0;
    double base = hex ? 16 : 10;
    bool have_digits = false;
    bool in_exp = false;
    char c;

    for (;;) {
        c = *cur;

        if (hex && ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
            if (in_exp) divisor *= base;
            else value *= base;

            value += (c <= '9' ? c - '0' : (c & ~0x20) - 'A' + 10) / divisor;
            have_digits = true;
        } else if (!hex && (c >= '0' && c <= '9')) {
            if (in_exp) divisor *= base;
            else value *= base;

            value += (c - '0') / divisor;
            have_digits = true;
        } else if (!in_exp && c == '.') {
            in_exp = true;
        } else {
            break;
        }

        cur++;
    }

    if (!have_digits) goto done;
    nptr = cur;

    if ((c & ~0x20) == (hex ? 'P' : 'E')) {
        c = *++cur;
        bool exp_neg = c == '-';
        if (exp_neg || c == '+') cur++;

        have_digits = false;
        unsigned exp_val = 0;

        for (;;) {
            c = *cur;

            if (c >= '0' && c <= '9') {
                exp_val = (exp_val * 10) + (c - '0');
                have_digits = true;
            } else {
                break;
            }

            cur++;
        }

        if (!have_digits) goto done;
        nptr = cur;

        for (unsigned i = 0; i < exp_val; i++) {
            if (exp_neg) value /= hex ? 2 : 10;
            else value *= hex ? 2 : 10;
        }
    }

done:
    if (endptr) *endptr = (char *)nptr;

    return negative ? -value : value;
}
