#include "compiler.h"
#include <setjmp.h>
#include <signal.h>
#include <stddef.h>

static void save_mask(sigjmp_buf buf, int should_save) {
    if (should_save) {
        sigprocmask(SIG_BLOCK, NULL, &buf->__sigmask);
        buf->__sigmask |= 1;
    } else {
        buf->__sigmask = 0;
    }
}

EXPORT __attribute__((naked)) int sigsetjmp(sigjmp_buf env, int savemask) {
    asm("mov %%rbx, %c1(%%rdi) \n\t"
        "mov %%rbp, %c2(%%rdi) \n\t"
        "mov %%r12, %c3(%%rdi) \n\t"
        "mov %%r13, %c4(%%rdi) \n\t"
        "mov %%r14, %c5(%%rdi) \n\t"
        "mov %%r15, %c6(%%rdi) \n\t"
        " \n\t"
        "lea 8(%%rsp), %%rax \n\t"
        "mov %%rax, %c7(%%rdi) \n\t"
        "mov (%%rsp), %%rax \n\t"
        "mov %%rax, %c8(%%rdi) \n\t"
        " \n\t"
        "sub $8, %%rsp \n\t" // Align stack for call
        "call %p0 \n\t"
        "add $8, %%rsp \n\t"
        " \n\t"
        "xor %%eax, %%eax \n\t"
        "ret" ::"i"(save_mask),
        "n"(offsetof(typeof(*env), __rbx)),
        "n"(offsetof(typeof(*env), __rbp)),
        "n"(offsetof(typeof(*env), __r12)),
        "n"(offsetof(typeof(*env), __r13)),
        "n"(offsetof(typeof(*env), __r14)),
        "n"(offsetof(typeof(*env), __r15)),
        "n"(offsetof(typeof(*env), __rsp)),
        "n"(offsetof(typeof(*env), __rip)));
}
