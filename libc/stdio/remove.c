#include "compiler.h"
#include "stdio.h"
#include <errno.h>
#include <unistd.h>

EXPORT int remove(const char *filename) {
    if (unlink(filename) && (errno != EISDIR || rmdir(filename))) return -1;
    return 0;
}
