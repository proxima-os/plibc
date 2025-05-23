#ifndef _FCNTL_H
#define _FCNTL_H 1

#include <bits/features.h>
#include <bits/types.h>
#include <hydrogen/fcntl.h>

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

#define O_RDONLY __O_RDONLY
#define O_WRONLY __O_WRONLY
#define O_RDWR (O_RDONLY | O_WRONLY)
#define O_ACCMODE O_RDWR

#define O_APPEND __O_APPEND
#define O_CREAT __O_CREAT
#define O_EXCL __O_EXCL
#define O_NOCTTY 0
#define O_NONBLOCK __O_NONBLOCK
#define O_TRUNC __O_TRUNC

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

#ifdef _PROXIMA_SOURCE

#define FD_CLOFORK (1 << 1)

#define O_CLOFORK __O_CLOFORK
#define O_DIRECTORY __O_DIRECTORY
#define O_NOFOLLOW __O_NOFOLLOW
#define O_CLOEXEC __O_CLOEXEC

#define AT_SYMLINK_FOLLOW __AT_SYMLINK_FOLLOW
#define AT_REMOVEDIR __AT_REMOVEDIR
#define AT_EACCESS _AT_EACCESS
#define AT_SYMLINK_NOFOLLOW __AT_SYMLINK_NOFOLLOW

#endif /* defined(_PROXIMA_SOURCE) */

#ifdef __cplusplus
};
#endif

#endif /* _FCNTL_H */
