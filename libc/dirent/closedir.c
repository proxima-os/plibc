#include "dirent.h"
#include "compiler.h"
#include <stdlib.h>
#include <unistd.h>

EXPORT int closedir(DIR *dirp) {
    int ret = close(dirp->__fd);
    free(dirp);
    return ret;
}
