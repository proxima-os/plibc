#ifndef _DIRENT_H
#define _DIRENT_H

#include <bits/features.h>
#include <bits/types.h>

#if _POSIX_C_SOURCE < 1
#error "dirent.h requires _POSIX_C_SOURCE >= 1"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int __fd;
    void *__buf_head;
    void *__buf_cur;
    void *__buf_end;
    void *__buf_tail;
    int __eof;
} DIR;

struct dirent {
    __size_t __size;
    __uint64_t __id;
    __size_t __name_length;
    int __type;
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
