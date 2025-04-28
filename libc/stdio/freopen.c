#include "compiler.h"
#include "stdio.h"
#include "stdio.p.h"
#include <errno.h>

EXPORT FILE *freopen(UNUSED const char *filename, UNUSED const char *mode, FILE *stream) {
    do_close(stream);

    errno = ENOENT;
    return NULL;
}
