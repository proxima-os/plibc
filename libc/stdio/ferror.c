#include "compiler.h"
#include "stdio.h"

EXPORT int ferror(FILE *stream) {
    return !!stream->__err;
}
