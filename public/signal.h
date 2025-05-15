#ifndef _SIGNAL_H
#define _SIGNAL_H 1

#include <bits/features.h>
#include <bits/types.h>
#include <hydrogen/signal.h>

#ifndef __PLIBC_SIGNAL_INLINE
#define __PLIBC_SIGNAL_INLINE static inline
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define SIG_ERR ((void(*)(int))-1)
#define SIG_DFL __SIG_DFL
#define SIG_IGN __SIG_IGN

#define SIGABRT __SIGABRT
#define SIGFPE __SIGFPE
#define SIGILL __SIGILL
#define SIGINT __SIGINT
#define SIGSEGV __SIGSEGV
#define SIGTERM __SIGTERM

typedef int sig_atomic_t;
typedef void (*__sig_handler_t)(int);

__sig_handler_t signal(int __sig, __sig_handler_t __func);
int raise(int __sig);

#if _POSIX_C_SOURCE >= 1

#define SA_NOCLDSTOP __SA_NOCLDSTOP

#define SIGALRM __SIGALRM
#define SIGCHLD __SIGCHLD
#define SIGCONT __SIGCONT
#define SIGHUP __SIGHUP
#define SIGKILL __SIGKILL
#define SIGPIPE __SIGPIPE
#define SIGQUIT __SIGQUIT
#define SIGSTOP __SIGSTOP
#define SIGTSTP __SIGTSTP
#define SIGTTIN __SIGTTIN
#define SIGTTOU __SIGTTOU
#define SIGUSR1 __SIGUSR1
#define SIGUSR2 __SIGUSR2

#define SIG_BLOCK __SIG_BLOCK
#define SIG_UNBLOCK __SIG_UNBLOCK
#define SIG_SETMASK __SIG_SETMASK

typedef __sigset_t sigset_t;

struct sigaction {
    struct __sigaction __base;
};

#define sa_handler __base.__func.__handler
#define sa_mask __base.__mask
#define sa_flags __base.__flags

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
#define NSIG __NSIG
#endif

#ifdef __cplusplus
};
#endif

#undef __PLIBC_SIGNAL_INLINE

#elif defined(__PLIBC_SIGNAL_INLINE)
#error "__PLIBC_SIGNAL_INLINE defined during later inclusion"
#endif /* _SIGNAL_H */
