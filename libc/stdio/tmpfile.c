#include "compiler.h"
#include "stdio.h"
#include <errno.h>
#include <stddef.h>

EXPORT FILE *tmpfile(void) {
    errno = ENOSYS;
    return NULL;
}
