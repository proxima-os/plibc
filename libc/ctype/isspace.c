#include "compiler.h"
#include "ctype.h"

EXPORT int isspace(int c) {
    return (c >= 0x09 && c <= 0x0d) || c == 0x20;
}
