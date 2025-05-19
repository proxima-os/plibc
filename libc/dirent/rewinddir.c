#include "compiler.h"
#include "dirent.h"
#include <unistd.h>

EXPORT void rewinddir(DIR *dirp) {
    lseek(dirp->__fd, 0, SEEK_SET);
    dirp->__buf_cur = dirp->__buf_end = dirp->__buf_head;
    dirp->__eof = 0;
}
