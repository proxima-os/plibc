#include "compiler.h"
#include "ctype.h"

EXPORT int islower(int c) {
    return c >= 0x61 && c <= 0x7a;
}
