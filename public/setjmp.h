#ifndef _SETJMP_H
#define _SETJMP_H

#ifdef __cplusplus
extern "C" {
#endif

/* C90 */

#define setjmp setjmp

/* rbx, rbp, r12, r13, r14, r15, rsp, rip */
typedef unsigned long jmp_buf[8];

__attribute__((__returns_twice__)) int setjmp(jmp_buf __env);
__attribute__((__noreturn__)) void longjmp(jmp_buf __env, int __val);

#ifdef __cplusplus
};
#endif

#endif /* _SETJMP_H */
