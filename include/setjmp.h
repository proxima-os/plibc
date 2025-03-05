#ifndef _SETJMP_H
#define _SETJMP_H 1

#include <bits/__sigset_t.h>
#include <bits/features.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long jmp_buf[7];

__attribute__((__returns_twice__)) int setjmp(jmp_buf __env);
__attribute__((__noreturn__)) void longjmp(jmp_buf __env, int __val);

#define setjmp setjmp

#ifdef _POSIX_C_SOURCE

typedef struct {
    jmp_buf __buf;
    __sigset_t __mask;
} sigjmp_buf[1];

__attribute__((__returns_twice__)) int sigsetjmp(sigjmp_buf __env, int __savemask);
__attribute__((__noreturn__)) void siglongjmp(sigjmp_buf __env, int __val);

#endif /* defined(_POSIX_C_SOURCE) */

#ifdef __cplusplus
};
#endif

#endif /* _SETJMP_H */
