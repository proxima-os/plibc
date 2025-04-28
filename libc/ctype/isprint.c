#include "compiler.h"
#include "ctype.h"

EXPORT int isprint(int c) {
    return c >= 0x20 && c <= 0x7e;
}
