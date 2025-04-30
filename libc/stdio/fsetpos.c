#include "compiler.h"
#include "stdio.h"
#include <errno.h>

EXPORT int fsetpos(FILE *stream, const fpos_t *pos) {
    errno = ESPIPE;
    return -1;
}
