#include "compiler.h"
#include "stdio.h"

EXPORT int getc(FILE *stream) {
    return fgetc(stream);
}
