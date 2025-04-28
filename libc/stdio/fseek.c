#include "compiler.h"
#include "stdio.h"
#include <errno.h>

EXPORT int fseek(UNUSED FILE *stream, UNUSED long offset, UNUSED int whence) {
    errno = ESPIPE;
    return -1;
}
