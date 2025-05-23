#pragma once

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define RTLD_OS(err) (-(err))

#define RTLD_DEPENDENCY_CYCLE 1
#define RTLD_INVALID_IMAGE 2
#define RTLD_NO_IMAGE_FOR_ADDRESS 3
#define RTLD_NO_ORIGIN 4
#define RTLD_NO_ORIGIN_SECURE 5
#define RTLD_UNRESOLVED_SYMBOL 6

extern int rtld_last_error;

typedef struct tcb {
    struct tcb *self;
    size_t dtv_size;
    void **dtv;
} tcb_t;

_Noreturn static inline void rtld_fatal(void) {
    fflush(stderr);
    abort();
}

__attribute__((format(printf, 1, 2))) _Noreturn static inline void panic(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    rtld_fatal();
}

void arch_rtld_set_tcb(tcb_t *tcb) asm("__plibc_rtld_set_tcb");
tcb_t *arch_rtld_get_tcb(void) asm("__plibc_rtld_get_tcb");

uintptr_t tls_static_module(void) asm("__plibc_rtld_tls_self_module");
