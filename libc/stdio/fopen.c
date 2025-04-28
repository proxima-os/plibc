#include "compiler.h"
#include "stdio.h"
#include <errno.h>

EXPORT FILE *fopen(UNUSED const char *filename, UNUSED const char *mode) {
    errno = ENOENT;
    return NULL;
}
