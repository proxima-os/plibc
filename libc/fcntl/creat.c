#include "compiler.h"
#include "fcntl.h"
#include <sys/types.h>

EXPORT int creat(const char *path, mode_t mode) {
    return open(path, O_WRONLY | O_TRUNC | O_CREAT, mode);
}
