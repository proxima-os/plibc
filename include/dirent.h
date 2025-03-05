#ifndef _DIRENT_H
#define _DIRENT_H 1

#include <bits/features.h>

#ifndef _POSIX_C_SOURCE
#error "dirent.h requires POSIX"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct dirent {
    char d_name[1];
};

typedef struct {
    char __x;
} DIR;

DIR *opendir(const char *__dirname);
struct dirent *readdir(DIR *__dirp);
void rewinddir(DIR *__dirp);
int closedir(DIR *__dirp);

#ifdef __cplusplus
};
#endif

#endif /* _DIRENT_H */
