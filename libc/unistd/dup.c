#include "compiler.h"
#include "unistd.h"
#include <fcntl.h>

EXPORT int dup(int fildes) {
    return fcntl(fildes, F_DUPFD, 0);
}
