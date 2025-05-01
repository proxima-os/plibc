#include "compiler.h"
#include "stdlib.p.h"
#include "unistd.h"

EXPORT int execv(const char *path, char *const *argv) {
    return execve(path, argv, environ);
}
