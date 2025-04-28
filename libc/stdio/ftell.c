#include "compiler.h"
#include "stdio.h"
#include <errno.h>

EXPORT long ftell(UNUSED FILE *stream) {
    errno = ESPIPE;
    return -1;
}
