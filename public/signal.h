#ifndef _SIGNAL_H
#define _SIGNAL_H 1

#include <bits/features.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SIG_DFL ((void(*)(int))0)
#define SIG_ERR ((void(*)(int))-1)
#define SIG_IGN ((void(*)(int))1)

typedef int sig_atomic_t;

/* when adding more signals, make sure to update NUM_SIGNALS in signal.c */
#define SIGABRT 1
#define SIGFPE 2
#define SIGILL 3
#define SIGINT 4
#define SIGSEGV 5
#define SIGTERM 6

void (*signal(int __sig, void (*__func)(int)))(int);
int raise(int __sig);

#ifdef __cplusplus
};
#endif

#endif /* _SIGNAL_H */
