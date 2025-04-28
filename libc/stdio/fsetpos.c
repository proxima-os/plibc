#include "compiler.h"
#include "stdio.h"
#include <errno.h>

EXPORT int fsetpos(UNUSED FILE *stream, UNUSED const fpos_t *pos) {
    errno = ESPIPE;
    return -1;
}
