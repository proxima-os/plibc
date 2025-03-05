#include "ctype.h"
#include "compiler.h"

/*
 * These functions only operate on values that fit in unsigned char and on EOF.
 * 0x00-0x08    iscntrl
 * 0x09-0x0d    iscntrl, isspace
 * 0x0e-0x1f    iscntrl
 * 0x20-0x20    isprint, isspace
 * 0x21-0x2f    isprint, isgraph, ispunct
 * 0x30-0x39    isprint, isgraph, isdigit
 * 0x3a-0x40    isprint, isgraph, ispunct
 * 0x41-0x46    isprint, isgraph, isalpha, isxdigit, isupper
 * 0x47-0x5a    isprint, isgraph, isalpha, isupper
 * 0x5b-0x60    isprint, isgraph, ispunct
 * 0x61-0x66    isprint, isgraph, isalpha, isxdigit, islower
 * 0x67-0x7a    isprint, isgraph, isalpha, islower
 * 0x7b-0x7e    isprint, isgraph, ispunct
 * 0x7f-0x7f    iscntrl
 */

// TODO: Locale support. For now this all assumes pure 7-bit ASCII.

EXPORT int isalnum(int c) {
    return isalpha(c) || isdigit(c);
}

EXPORT int isalpha(int c) {
    c &= ~0x20;
    return c >= 0x41 && c <= 0x5a;
}

EXPORT int iscntrl(int c) {
    return (c >= 0x00 && c <= 0x1f) || c == 0x7f;
}

EXPORT int isdigit(int c) {
    return c >= 0x30 && c <= 0x39;
}

EXPORT int isgraph(int c) {
    return c != 0x20 && isprint(c);
}

EXPORT int islower(int c) {
    return c >= 0x61 && c <= 0x7a;
}

EXPORT int isprint(int c) {
    return c >= 0x20 && c <= 0x7e;
}

EXPORT int ispunct(int c) {
    return isgraph(c) && !isalnum(c);
}

EXPORT int isspace(int c) {
    return c == 0x20 || (c >= 0x09 && c <= 0x0d);
}

EXPORT int isupper(int c) {
    return c >= 0x41 && c <= 0x5a;
}

EXPORT int isxdigit(int c) {
    if (isdigit(c)) return 1;

    c &= ~0x20;
    return c >= 0x41 && c <= 0x46;
}

EXPORT int tolower(int c) {
    return isupper(c) ? c | 0x20 : c;
}

EXPORT int toupper(int c) {
    return islower(c) ? c & ~0x20 : c;
}
