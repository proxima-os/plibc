#include "compiler.h"
#include "ctype.h"

EXPORT int isalpha(int c) {
    return (c >= 0x41 && c <= 0x5a) || (c >= 0x61 && c <= 0x7a);
}
