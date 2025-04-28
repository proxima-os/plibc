#include "compiler.h"
#include "ctype.h"

EXPORT int isgraph(int c) {
    return c >= 0x21 && c <= 0x7e;
}
