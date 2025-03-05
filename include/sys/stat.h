#ifndef _SYS_STAT_H
#define _SYS_STAT_H 1

#include <bits/__dev_t.h>
#include <bits/__gid_t.h>
#include <bits/__ino_t.h>
#include <bits/__mode_t.h>
#include <bits/__nlink_t.h>
#include <bits/__off_t.h>
#include <bits/__time_t.h>
#include <bits/__uid_t.h>
#include <bits/features.h>

#ifndef _POSIX_C_SOURCE
#error "sys/stat.h requires POSIX"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct stat {
    __mode_t st_mode;
    __ino_t st_ino;
    __dev_t st_dev;
    __nlink_t st_nlink;
    __uid_t st_uid;
    __gid_t st_gid;
    __off_t st_size;
    __time_t st_atime;
    __time_t st_mtime;
    __time_t st_ctime;
};

#define S_IXOTH 0x001
#define S_IWOTH 0x002
#define S_IROTH 0x004
#define S_IXGRP 0x008
#define S_IWGRP 0x010
#define S_IRGRP 0x020
#define S_IXUSR 0x040
#define S_IWUSR 0x080
#define S_IRUSR 0x100
#define S_ISGID 0x400
#define S_ISUID 0x800

#define S_IRWXO (S_IROTH | S_IWOTH | S_IXOTH)
#define S_IRWXG (S_IRGRP | S_IWGRP | S_IXGRP)
#define S_IRWXU (S_IRUSR | S_IWUSR | S_IXUSR)

#define S_ISDIR(m) (((m) & 0xf000) == 0x1000)
#define S_ISCHR(m) (((m) & 0xf000) == 0x2000)
#define S_ISBLK(m) (((m) & 0xf000) == 0x3000)
#define S_ISREG(m) (((m) & 0xf000) == 0x4000)
#define S_ISFIFO(m) (((m) & 0xf000) == 0x5000)

__mode_t umask(__mode_t __cmask);
int mkdir(const char *__path, __mode_t __mode);
int mkfifo(const char *__path, __mode_t __mode);
int stat(const char *__restrict __path, struct stat *__restrict __buf);
int fstat(int __fildes, struct stat *__buf);
int chmod(const char *__path, __mode_t __mode);

#ifdef __cplusplus
};
#endif

#endif /* _SYS_STAT_H */
