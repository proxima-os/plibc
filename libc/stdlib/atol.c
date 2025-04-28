#include "compiler.h"
#include "stdlib.h"

EXPORT long atol(const char *str) {
    return strtol(str, NULL, 10);
}
