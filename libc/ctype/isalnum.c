#include "compiler.h"
#include "ctype.h"

EXPORT int isalnum(int c) {
    return (c >= 0x30 && c <= 0x39) || (c >= 0x41 && c <= 0x5a) || (c >= 0x61 && c <= 0x7a);
}
