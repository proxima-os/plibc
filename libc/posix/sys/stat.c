#include "sys/stat.h"
#include "compiler.h"
#include "stub.h"
#include <sys/types.h>

EXPORT mode_t umask(mode_t cmask) {
    STUB();
    return 0;
}

EXPORT int mkdir(const char *path, mode_t mode) {
    STUB();
    return -1;
}

EXPORT int mkfifo(const char *path, mode_t mode) {
    STUB();
    return -1;
}

EXPORT int stat(const char *restrict path, struct stat *restrict buf) {
    STUB();
    return -1;
}

EXPORT int fstat(int fildes, struct stat *buf) {
    STUB();
    return -1;
}

EXPORT int chmod(const char *path, __mode_t mode) {
    STUB();
    return -1;
}
