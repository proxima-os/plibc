#include "compiler.h"
#include "stdio.h"
#include <errno.h>

EXPORT int fgetpos(UNUSED FILE *stream, UNUSED fpos_t *pos) {
    errno = ESPIPE;
    return -1;
}
