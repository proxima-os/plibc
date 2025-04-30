#include "compiler.h"
#include "stdio.h"
#include <errno.h>

EXPORT FILE *fopen(const char *filename, const char *mode) {
    errno = ENOENT;
    return NULL;
}
