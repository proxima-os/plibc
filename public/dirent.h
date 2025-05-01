#ifndef _DIRENT_H
#define _DIRENT_H

#include <bits/features.h>

#if _POSIX_C_SOURCE < 1
#error "dirent.h requires _POSIX_C_SOURCE >= 1"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _DIR DIR;

struct dirent {
    char d_name[0];
};

int closedir(DIR *__dirp);
DIR *opendir(const char *__dirname);
struct dirent *readdir(DIR *__dirp);
void rewinddir(DIR *__dirp);

#ifdef __cplusplus
};
#endif

#endif /* _DIRENT_H */
