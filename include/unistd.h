#ifndef _UNISTD_H
#define _UNISTD_H 1

#include <bits/__gid_t.h>
#include <bits/__off_t.h>
#include <bits/__pid_t.h>
#include <bits/__uid_t.h>
#include <bits/features.h>
#include <bits/ssize_t.h> /* IWYU pragma: export */
#include <bits/whence.h>  /* IWYU pragma: export */

#ifndef _POSIX_C_SOURCE
#error "unistd.h requires POSIX"
#endif

#if _POSIX_C_SOURCE >= 199506L
#include <bits/intptr_t.h>
#include <bits/off_t.h>
#endif /* _POSIX_C_SOURCE >= 199506L */

#define __need_size_t
#define __need_NULL
#include <stddef.h> /* IWYU pragma: export */

#ifdef __cplusplus
extern "C" {
#endif

#define F_OK 0
#define R_OK (1 << 0)
#define W_OK (1 << 1)
#define X_OK (1 << 2)

#define _POSIX_JOB_CONTROL 1
#define _POSIX_SAVED_IDS 1
#define _POSIX_VERSION 199009L

#define _POSIX_CHOWN_RESTRICTED 1
#define _POSIX_NO_TRUNC 1
#define _POSIX_VDISABLE '\0'

#define _PC_CHOWN_RESTRICTED 0
#define _PC_LINK_MAX 1
#define _PC_MAX_CANON 2
#define _PC_MAX_INPUT 3
#define _PC_NAME_MAX 4
#define _PC_NO_TRUNC 5
#define _PC_PATH_MAX 6
#define _PC_PIPE_BUF 7
#define _PC_VDISABLE 8

#define _SC_ARG_MAX 0
#define _SC_CHILD_MAX 1
#define _SC_CLK_TCK 2
#define _SC_NGROUPS_MAX 3
#define _SC_OPEN_MAX 4
#define _SC_STREAM_MAX 5
#define _SC_TZNAME_MAX 6
#define _SC_JOB_CONTROL 7
#define _SC_SAVED_IDS 8
#define _SC_VERSION 9

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

__pid_t fork(void);
int execl(const char *__path, const char *__arg, ...);
int execv(const char *__path, char *const __argv[]);
int execle(const char *__path, const char *__arg, ...);
int execve(const char *__path, char *const __argv[], char *const __envp[]);
int execlp(const char *__file, const char *__arg, ...);
int execvp(const char *__file, char *const __argv[]);
__attribute__((__noreturn__)) void _exit(int __status);
unsigned alarm(unsigned __seconds);
int pause(void);
unsigned sleep(unsigned __seconds);
__pid_t getpid(void);
__pid_t getppid(void);
__uid_t getuid(void);
__uid_t geteuid(void);
__gid_t getgid(void);
__gid_t getegid(void);
int setuid(__uid_t __uid);
int setgid(__gid_t __gid);
int getgroups(int __gidsetsize, __gid_t __grouplist[]);
char *getlogin(void);
__pid_t getpgrp(void);
__pid_t setsid(void);
int setpgid(__pid_t __pid, __pid_t __pgid);
char *ttyname(int __fildes);
int isatty(int __fildes);
long sysconf(int __name);
int chdir(const char *__path);
char *getcwd(char *__buf, size_t __size);
int link(const char *__existing, const char *__new);
int unlink(const char *__path);
int rmdir(const char *__path);
int access(const char *__path, int __amode);
int chown(const char *__path, __uid_t __owner, __gid_t __group);
long pathconf(const char *__path, int __name);
long fpathconf(int __fildes, int __name);
int pipe(int __fildes[2]);
int dup(int __fildes);
int dup2(int __fildes);
int close(int __fildes);
ssize_t read(int __fildes, void *__buf, size_t __nbyte);
ssize_t write(int __fildes, const void *__buf, size_t __nbyte);
__off_t lseek(int __fildes, __off_t __offset, int __whence);
__pid_t tcgetpgrp(int __fildes);
int tcsetpgrp(int __fildes, __pid_t __pgrp_id);

#ifdef __cplusplus
};
#endif

#endif /* _UNISTD_H */
