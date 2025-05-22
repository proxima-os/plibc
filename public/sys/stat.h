#ifndef _SYS_STAT_H
#define _SYS_STAT_H 1

#include <bits/features.h>
#include <bits/types.h>
#include <hydrogen/stat.h>

#if _POSIX_C_SOURCE < 1
#error "sys/stat.h requires _POSIX_C_SOURCE >= 1"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define S_IXOTH __S_IXOTH
#define S_IWOTH __S_IWOTH
#define S_IROTH __S_IROTH
#define S_IRWXO (S_IROTH | S_IWOTH | S_IXOTH)
#define S_IXGRP __S_IXGRP
#define S_IWGRP __S_IWGRP
#define S_IRGRP __S_IRGRP
#define S_IRWXG (S_IRGRP | S_IWGRP | S_IXGRP)
#define S_IXUSR __S_IXUSR
#define S_IWUSR __S_IWUSR
#define S_IRUSR __S_IRUSR
#define S_IRWXU (S_IRUSR | S_IWUSR | S_IXUSR)
#define S_ISGID __S_ISGID
#define S_ISUID __S_ISUID

#define __S_IFMT 0xf000
#define __S_IFIFO 0x1000
#define __S_IFCHR 0x2000
#define __S_IFDIR 0x4000
#define __S_IFBLK 0x6000
#define __S_IFREG 0x8000
#define __S_IFLNK 0xa000

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
    __mode_t st_mode;
    __uid_t st_uid;
    __gid_t st_gid;
};

#define st_atime st_atim.tv_sec
#define st_ctime st_ctim.tv_sec
#define st_mtime st_mtim.tv_sec

int chmod(const char *__path, __mode_t __mode);
int fstat(int __fildes, struct stat *__buf);
int mkdir(const char *__path, __mode_t __mode);
int mkfifo(const char *__path, __mode_t __mode);
int stat(const char *__restrict __path, struct stat *__restrict __buf);
__mode_t umask(__mode_t __cmask);

#ifdef _PROXIMA_SOURCE
#define S_ISVTX __S_ISVTX
#define S_IFMT __S_IFMT
#define S_IFIFO __S_IFIFO
#define S_IFCHR __S_IFCHR
#define S_IFDIR __S_IFDIR
#define S_IFBLK __S_IFBLK
#define S_IFREG __S_IFREG
#define S_IFLNK __S_IFLNK
#endif /* defined(_PROXIMA_SOURCE) */

#ifdef __cplusplus
};
#endif

#endif /* _SYS_STAT_H */
