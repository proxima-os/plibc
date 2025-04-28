#include "compiler.h"
#include "stdio.h"
#include <errno.h>

EXPORT void rewind(FILE *stream) {
    errno = ESPIPE;
    stream->__err = 0;
}
