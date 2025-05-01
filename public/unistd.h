#ifndef _UNISTD_H
#define _UNISTD_H 1

#include <bits/NULL.h>
#include <bits/features.h>
#include <bits/seek.h>
#include <bits/size_t.h>
#include <bits/ssize_t.h>
#include <bits/types.h>

#if _POSIX_C_SOURCE < 1
#error "unistd.h requires _POSIX_C_SOURCE >= 1"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define _POSIX_CHOWN_RESTRICTED 1
#define _POSIX_JOB_CONTROL 1
#define _POSIX_NO_TRUNC 1
#define _POSIX_SAVED_IDS 1
#define _POSIX_VDISABLE '\0'
#define _POSIX_VERSION 199009L

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
#define _SC_JOB_CONTROL 3
#define _SC_NGROUPS_MAX 4
#define _SC_OPEN_MAX 5
#define _SC_SAVED_IDS 6
#define _SC_STREAM_MAX 7
#define _SC_TZNAME_MAX 8
#define _SC_VERSION 9

#define F_OK 0
#define R_OK (1 << 0)
#define W_OK (1 << 1)
#define X_OK (1 << 2)

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

__attribute__((__noreturn__)) void _exit(int __status);
int access(const char *__path, int __amode);
unsigned alarm(unsigned __seconds);
int chdir(const char *__path);
int chown(const char *__path, __uid_t __owner, __gid_t __group);
int close(int __fildes);
char *ctermid(char *__s);
int dup2(int __fildes, int __fildes2);
int dup(int __fildes);
int execl(const char *__path, const char *__arg, ...);
int execle(const char *__path, const char *__arg, ...);
int execlp(const char *__path, const char *__arg, ...);
int execv(const char *__path, char *const __argv[]);
int execve(const char *__path, char *const __argv[], char *const __envp[]);
int execvp(const char *__path, char *const __argv[]);
__pid_t fork(void);
long fpathconf(int __fildes, int __name);
char *getcwd(char *__buf, size_t __size);
__gid_t getegid(void);
__uid_t geteuid(void);
__gid_t getgid(void);
int getgroups(int __gidsetsize, __gid_t __grouplist[]);
char *getlogin(void);
__pid_t getpgrp(void);
__pid_t getpid(void);
__pid_t getppid(void);
__uid_t getuid(void);
int isatty(int __fildes);
int link(const char *__existing, const char *__new);
__off_t lseek(int __fildes, __off_t __offset, int __whence);
long pathconf(const char *__path, int __name);
int pause(void);
int pipe(int __fildes[2]);
ssize_t read(int __fildes, void *__buf, size_t __nbyte);
int rmdir(const char *__path);
int setgid(__gid_t __gid);
int setpgid(__pid_t __pid, __pid_t __pgid);
__pid_t setsid(void);
int setuid(__uid_t __uid);
unsigned sleep(unsigned __seconds);
long sysconf(int __name);
__pid_t tcgetpgrp(int __fildes);
int tcsetpgrp(__pid_t __fildes, int __pgrp_id);
char *ttyname(int __fildes);
int unlink(const char *__path);
ssize_t write(int __fildes, const void *__buf, size_t __nbyte);

#ifdef __cplusplus
};
#endif

#endif /* _UNISTD_H */
