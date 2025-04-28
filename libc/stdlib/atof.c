#include "compiler.h"
#include "stdlib.h"
#include <stddef.h>

EXPORT double atof(const char *str) {
    return strtod(str, NULL);
}
