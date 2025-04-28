#include "compiler.h"
#include "stdio.h"

EXPORT void setbuf(FILE *stream, char *buf) {
    setvbuf(stream, buf, buf ? _IOFBF : _IONBF, BUFSIZ);
}
