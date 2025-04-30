#include "compiler.h"
#include "stdlib.h"
#include <errno.h>

EXPORT int system(const char *string) {
    errno = ENOENT;
    return -1;
}
