#ifndef _SIGNAL_H
#define _SIGNAL_H 1

#include <bits/features.h>
#include <bits/types.h>

#ifndef __PLIBC_SIGNAL_INLINE
#define __PLIBC_SIGNAL_INLINE static inline
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define SIG_DFL ((void(*)(int))0)
#define SIG_ERR ((void(*)(int))-1)
#define SIG_IGN ((void(*)(int))1)

/* when adding more signals, make sure to update NSIG */
#define SIGABRT 1
#define SIGFPE 2
#define SIGILL 3
#define SIGINT 4
#define SIGSEGV 5
#define SIGTERM 6

typedef int sig_atomic_t;
typedef void (*__sig_handler_t)(int);

__sig_handler_t signal(int __sig, __sig_handler_t __func);
int raise(int __sig);

#if _POSIX_C_SOURCE >= 1

#define SA_NOCLDSTOP (1 << 0)

#define SIGALRM 7
#define SIGCHLD 8
#define SIGCONT 9
#define SIGHUP 10
#define SIGKILL 11
#define SIGPIPE 12
#define SIGQUIT 13
#define SIGSTOP 14
#define SIGTSTP 15
#define SIGTTIN 16
#define SIGTTOU 17
#define SIGUSR1 18
#define SIGUSR2 19

#define SIG_BLOCK 0
#define SIG_UNBLOCK 1
#define SIG_SETMASK 2

typedef __sigset_t sigset_t;

struct sigaction {
    __sig_handler_t sa_handler;
    sigset_t sa_mask;
    int sa_flags;
};

int kill(__pid_t __pid, int __sig);
int sigaction(int __sig, const struct sigaction *__restrict __act, struct sigaction *__restrict __oact);
int sigpending(sigset_t *__set);
int sigprocmask(int __how, const sigset_t *__restrict __set, sigset_t *__restrict __oset);
int sigsuspend(const sigset_t *__sigmask);

__PLIBC_SIGNAL_INLINE int sigaddset(sigset_t *__set, int __signo) {
    *__set |= 1ull << __signo;
    return 0;
}

__PLIBC_SIGNAL_INLINE int sigdelset(sigset_t *__set, int __signo) {
    *__set &= ~(1ull << __signo);
    return 0;
}

__PLIBC_SIGNAL_INLINE int sigemptyset(sigset_t *__set) {
    *__set = 0;
    return 0;
}

__PLIBC_SIGNAL_INLINE int sigfillset(sigset_t *__set) {
    *__set = -1;
    return 0;
}

__PLIBC_SIGNAL_INLINE int sigismember(const sigset_t *__set, int __signo) {
    return !!(*__set & (1ull << __signo));
}

#endif /* _POSIX_C_SOURCE >= 1 */

#ifdef _PROXIMA_SOURCE
#define NSIG 20
#endif

#ifdef __cplusplus
};
#endif

#undef __PLIBC_SIGNAL_INLINE

#elif defined(__PLIBC_SIGNAL_INLINE)
#error "__PLIBC_SIGNAL_INLINE defined during later inclusion"
#endif /* _SIGNAL_H */
