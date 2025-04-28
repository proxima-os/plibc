#include "compiler.h"
#include "string.h"
#include <stddef.h>

EXPORT size_t strspn(const char *s1, const char *s2) {
    size_t len = 0;

    for (;;) {
        char c1 = *s1++;
        char c2 = *s2++;
        if (!c1 || !c2 || c1 != c2) return len;
        len++;
    }
}
