#include "compiler.h"
#include "string.h"
#include <stddef.h>

#undef strlen

EXPORT size_t strlen(const char *s) {
    size_t len = 0;
    while (*s++) len++;
    return len;
}
