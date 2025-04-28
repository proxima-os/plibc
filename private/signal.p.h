#pragma once

#define NUM_SIGNALS 7

extern void (*sighand[NUM_SIGNALS])(int) asm("__plibc_sighand");
