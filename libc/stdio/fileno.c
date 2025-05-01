#include "compiler.h"
#include "stdio.h"

EXPORT int fileno(FILE *stream) {
    return stream->__fd;
}
