#include "compiler.h"
#include "stdio.h"
#include <errno.h>

EXPORT int rename(UNUSED const char *old, UNUSED const char *new) {
    errno = ENOENT;
    return -1;
}
