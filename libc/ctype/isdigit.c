#include "compiler.h"
#include "ctype.h"

EXPORT int isdigit(int c) {
    return c >= 0x30 && c <= 0x39;
}
