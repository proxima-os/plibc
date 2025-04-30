#include "compiler.h"
#include "stdio.h"
#include <errno.h>

EXPORT int fseek(FILE *stream, long offset, int whence) {
    errno = ESPIPE;
    return -1;
}
