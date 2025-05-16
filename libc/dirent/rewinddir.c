#include "compiler.h"
#include "dirent.h"
#include <unistd.h>

EXPORT void rewinddir(DIR *dirp) {
    lseek(dirp->__fd, 0, SEEK_SET);
}
