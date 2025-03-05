#include "dirent.h"
#include "compiler.h"
#include "stub.h"
#include <stddef.h>

EXPORT DIR *opendir(const char *dirname) {
    STUB();
    return NULL;
}

EXPORT struct dirent *readdir(DIR *dirp) {
    STUB();
    return NULL;
}

EXPORT void rewinddir(DIR *dirp) {
    STUB();
}

EXPORT int closedir(DIR *dirp) {
    STUB();
    return -1;
}
