#include "compiler.h"
#include "stdio.h"
#include <errno.h>

EXPORT int rename(const char *old, const char *new) {
    errno = ENOENT;
    return -1;
}
