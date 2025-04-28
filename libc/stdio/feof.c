#include "compiler.h"
#include "stdio.h"

EXPORT int feof(FILE *stream) {
    return !!stream->__eof;
}
