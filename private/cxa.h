#pragma once

typedef struct atexit {
    struct atexit *next;
    void (*func)(void *);
    void *param;
    void *dso;
} atexit_t;

extern atexit_t *atexits asm("__plibc_atexits");

int __cxa_atexit(void (*func)(void *), void *param, void *dso);
void __cxa_finalize(void *dso);
