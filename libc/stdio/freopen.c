#include "compiler.h"
#include "stdio.h"
#include "stdio.p.h"
#include <errno.h>

EXPORT FILE *freopen(const char *filename, const char *mode, FILE *stream) {
    do_close(stream);

    errno = ENOENT;
    return NULL;
}
