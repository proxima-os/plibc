#include "compiler.h"
#include "ctype.h"

EXPORT int iscntrl(int c) {
    return (c >= 0x00 && c <= 0x1f) || c == 0x7f;
}
