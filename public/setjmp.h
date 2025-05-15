#ifndef _SETJMP_H
#define _SETJMP_H

#include <bits/features.h>
#include <bits/types.h>
#include <hydrogen/signal.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    __size_t __rbx;
    __size_t __rbp;
    __size_t __r12;
    __size_t __r13;
    __size_t __r14;
    __size_t __r15;
    __size_t __rsp;
    __size_t __rip;
    __sigset_t __sigmask;
} jmp_buf[1];

__attribute__((__returns_twice__)) int setjmp(jmp_buf __env);
__attribute__((__noreturn__)) void longjmp(jmp_buf __env, int __val);

#define setjmp(env) setjmp(env)

#if _POSIX_C_SOURCE >= 1

typedef jmp_buf sigjmp_buf;

__attribute__((__returns_twice__)) int sigsetjmp(sigjmp_buf __env, int __savemask);
__attribute__((__noreturn__)) void siglongjmp(sigjmp_buf __env, int __val);

#endif /* _POSIX_C_SOURCE >= 1 */

#ifdef __cplusplus
};
#endif

#endif /* _SETJMP_H */
