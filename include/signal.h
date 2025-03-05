#ifndef _SIGNAL_H
#define _SIGNAL_H 1

#ifndef _PLIBC_INLINE_IMPL
#define _PLIBC_INLINE_IMPL static __inline
#endif

#include <bits/__sigset_t.h>
#include <bits/features.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int sig_atomic_t;
typedef void (*__sig_handler_t)(int);

#define SIG_ERR ((void (*)(int)) - 1)
#define SIG_DFL ((void (*)(int))0)
#define SIG_IGN ((void (*)(int))1)

/* all signal numbers must be between 1 and 0x7f */
#define SIGABRT 1
#define SIGFPE 2
#define SIGILL 3
#define SIGINT 4
#define SIGSEGV 5
#define SIGTERM 6

__sig_handler_t signal(int __sig, __sig_handler_t __func);
int raise(int __sig);

#ifdef _POSIX_C_SOURCE

#include <bits/__pid_t.h>

typedef __sigset_t sigset_t;

struct sigaction {
    __sig_handler_t sa_handler;
    sigset_t sa_mask;
    int sa_flags;
};

#define SA_NOCLDSTOP (1 << 0)

#define SIG_BLOCK 0
#define SIG_UNBLOCK 1
#define SIG_SETMASK 2

#define SIGALRM 7
#define SIGHUP 8
#define SIGKILL 9
#define SIGPIPE 10
#define SIGQUIT 11
#define SIGUSR1 12
#define SIGUSR2 13
#define SIGCHLD 14
#define SIGCONT 15
#define SIGSTOP 16
#define SIGTSTP 17
#define SIGTTIN 18
#define SIGTTOU 19

__pid_t kill(__pid_t __pid, int __sig);
int sigaction(int __sig, const struct sigaction *__restrict __act, struct sigaction *__restrict __oact);
int sigprocmask(int __how, const sigset_t *__restrict __set, sigset_t *__restrict __oset);
int sigpending(sigset_t *__set);
int sigsuspend(const sigset_t *__set);

_PLIBC_INLINE_IMPL int sigemptyset(sigset_t *__set) {
    *__set = 0;
    return 0;
}

_PLIBC_INLINE_IMPL int sigfillset(sigset_t *__set) {
    *__set = -1;
    return 0;
}

_PLIBC_INLINE_IMPL int sigaddset(sigset_t *__set, int __signo) {
    *__set |= 1ul << __signo;
    return 0;
}

_PLIBC_INLINE_IMPL int sigdelset(sigset_t *__set, int __signo) {
    *__set &= ~(1ul << __signo);
    return 0;
}

_PLIBC_INLINE_IMPL int sigismember(const sigset_t *__set, int __signo) {
    return !!(*__set & (1ul << __signo));
}

#endif /* defined(_POSIX_C_SOURCE) */

#ifdef __cplusplus
};
#endif

#undef _PLIBC_INLINE_IMPL

#elif defined(_PLIBC_INLINE_IMPL)
#error "_PILBC_INLINE_IMPL is set for a guarded header"
#endif /* _SIGNAL_H */
