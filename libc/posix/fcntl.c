#include "fcntl.h"
#include "compiler.h"
#include "stub.h"
#include <sys/types.h>

EXPORT int open(const char *path, int oflag, ...) {
    STUB();
    return -1;
}

EXPORT int creat(const char *path, mode_t mode) {
    return open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
}

EXPORT int fcntl(int fildes, int cmd, ...) {
    STUB();
    return -1;
}
