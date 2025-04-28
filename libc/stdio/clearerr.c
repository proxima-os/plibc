#include "compiler.h"
#include "stdio.h"

EXPORT void clearerr(FILE *stream) {
    stream->__err = 0;
}
