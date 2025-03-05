#ifndef _FCNTL_H
#define _FCNTL_H 1

#include <bits/__mode_t.h>
#include <bits/__off_t.h>
#include <bits/__pid_t.h>
#include <bits/features.h>

#ifndef _POSIX_C_SOURCE
#error "fcntl.h requires POSIX"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct flock {
    short l_type;
    short l_whence;
    __off_t l_start;
    __off_t l_len;
    __pid_t l_pid;
};

#define O_RDONLY (1 << 0)
#define O_WRONLY (1 << 1)
#define O_RDWR (O_RDONLY | O_WRONLY)
#define O_APPEND (1 << 2)
#define O_CREAT (1 << 3)
#define O_EXCL (1 << 4)
#define O_NOCTTY (1 << 5)
#define O_NONBLOCK (1 << 6)
#define O_TRUNC (1 << 7)

#define O_ACCMODE O_RDWR

#define F_DUPFD 0
#define F_GETFD 1
#define F_GETLK 2
#define F_SETFD 3
#define F_GETFL 4
#define F_SETFL 5
#define F_SETLK 6
#define F_SETLKW 7

#define FD_CLOEXEC (1 << 0)

#define F_RDLCK 0
#define F_UNLCK 1
#define F_WRLCK 2

int open(const char *__path, int __oflag, ...);
int creat(const char *__path, __mode_t __mode);
int fcntl(int __fildes, int __cmd, ...);

#ifdef __cplusplus
};
#endif

#endif /* _FCNTL_H */
