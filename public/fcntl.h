#ifndef _FCNTL_H
#define _FCNTL_H 1

#include <bits/features.h>
#include <bits/types.h>

#if _POSIX_C_SOURCE < 1
#error "fcntl.h requires _POSIX_C_SOURCE >= 1"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define FD_CLOEXEC (1 << 0)

#define F_DUPFD 0
#define F_GETFD 1
#define F_GETFL 2
#define F_GETLK 3
#define F_SETFD 4
#define F_SETFL 5
#define F_SETLK 6
#define F_SETLKW 7

#define F_UNLCK 0
#define F_RDLCK 1
#define F_WRLCK 2

#define O_ACCMODE (7 << 0)
#define O_RDONLY (1 << 0)
#define O_WRONLY (1 << 1)
#define O_RDWR (O_RDONLY | O_WRONLY)

#define O_APPEND (1 << 3)
#define O_CREAT (1 << 4)
#define O_EXCL (1 << 5)
#define O_NOCTTY (1 << 6)
#define O_NONBLOCK (1 << 7)
#define O_TRUNC (1 << 8)

struct flock {
    __off_t l_start;
    __off_t l_len;
    __pid_t l_pid;
    short l_type;
    short l_whence;
};

int creat(const char *__path, __mode_t __mode);
int fcntl(int __fildes, int __cmd, ...);
int open(const char *__path, int __oflag, ...);

#ifdef __cplusplus
};
#endif

#endif /* _FCNTL_H */
