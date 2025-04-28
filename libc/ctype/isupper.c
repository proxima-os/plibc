#include "compiler.h"
#include "ctype.h"

EXPORT int isupper(int c) {
    return c >= 0x41 && c <= 0x5a;
}
