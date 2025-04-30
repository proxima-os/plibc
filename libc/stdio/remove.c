#include "compiler.h"
#include "stdio.h"
#include <errno.h>

EXPORT int remove(const char *filename) {
    errno = ENOENT;
    return -1;
}
