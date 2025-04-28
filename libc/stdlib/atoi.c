#include "compiler.h"
#include "stdlib.h"

EXPORT int atoi(const char *str) {
    return strtol(str, NULL, 10);
}
