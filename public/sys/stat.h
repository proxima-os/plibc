#ifndef _SYS_STAT_H
#define _SYS_STAT_H 1

#include <bits/features.h>
#include <bits/types.h>

#if _POSIX_C_SOURCE < 1
#error "sys/stat.h requires _POSIX_C_SOURCE >= 1"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define S_IXOTH 0000001
#define S_IWOTH 0000002
#define S_IROTH 0000004
#define S_IRWXO 0000007
#define S_IXGRP 0000010
#define S_IWGRP 0000020
#define S_IRGRP 0000040
#define S_IRWXG 0000070
#define S_IXUSR 0000100
#define S_IWUSR 0000200
#define S_IRUSR 0000400
#define S_IRWXU 0000700
#define S_ISGID 0002000
#define S_ISUID 0004000

#define __S_IFMT 0xf000
#define __S_IFIFO 0x1000
#define __S_IFCHR 0x2000
#define __S_IFDIR 0x4000
#define __S_IFBLK 0x6000
#define __S_IFREG 0x8000

#define S_ISFIFO(m) (((m) & __S_IFMT) == __S_IFIFO)
#define S_ISCHR(m) (((m) & __S_IFMT) == __S_IFCHR)
#define S_ISDIR(m) (((m) & __S_IFMT) == __S_IFDIR)
#define S_ISBLK(m) (((m) & __S_IFMT) == __S_IFBLK)
#define S_ISREG(m) (((m) & __S_IFMT) == __S_IFREG)

struct stat {
    __dev_t st_dev;
    __ino_t st_ino;
    __nlink_t st_nlink;
    __off_t st_size;
    struct __timespec st_atim;
    struct __timespec st_ctim;
    struct __timespec st_mtim;
    __uid_t uid;
    __gid_t gid;
};

int chmod(const char *__path, __mode_t __mode);
int fstat(int __fildes, struct stat *__buf);
int mkdir(const char *__path, __mode_t __mode);
int mkfifo(const char *__path, __mode_t __mode);
int stat(const char *__restrict __path, struct stat *__restrict __buf);
__mode_t umask(__mode_t __cmask);

#ifdef __cplusplus
};
#endif

#endif /* _SYS_STAT_H */
