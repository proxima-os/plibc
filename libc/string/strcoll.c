#include "compiler.h"
#include "string.h"

EXPORT int strcoll(const char *s1, const char *s2) {
    return strcmp(s1, s2);
}
