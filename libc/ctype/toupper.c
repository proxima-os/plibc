#include "compiler.h"
#include "ctype.h"

EXPORT int toupper(int c) {
    return (c >= 0x61 && c <= 0x7a) ? (c - 0x20) : c;
}
