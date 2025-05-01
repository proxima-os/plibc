#include "compiler.h"
#include "stdio.h"

EXPORT void rewind(FILE *stream) {
    fseek(stream, 0, SEEK_SET);
    stream->__err = 0;
}
