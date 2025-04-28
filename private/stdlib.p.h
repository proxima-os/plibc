#pragma once

#include <stddef.h>
#include <stdint.h>

#define MT_w 32
#define MT_n 624
#define MT_m 397
#define MT_r 31
#define MT_a 0x9908b0dfUL
#define MT_u 11
#define MT_d 0xffffffffUL
#define MT_s 7
#define MT_b 0x9d2c5680UL
#define MT_t 15
#define MT_c 0xefc60000UL
#define MT_l 18
#define MT_f 1812433253UL

#define MT_UMASK (0xffffffffUL << (MT_r))
#define MT_LMASK (0xffffffffUL >> (MT_w - MT_r))

#define ZERO_PTR ((void *)_Alignof(max_align_t))
#define ALLOC_HUGE 0x1000
#define HUGE_ALIGN(x) (((x) + (ALLOC_HUGE - 1)) & ~(ALLOC_HUGE - 1))
#define ALLOC_BUCKET(x) (64 - __builtin_clzl((x) - 1))
#define MAX_BUCKET ALLOC_BUCKET(ALLOC_HUGE - 1)

typedef struct {
    uint32_t data[MT_n];
    int index;
} mt_state_t;

typedef struct {
    size_t size;
} alloc_meta_t;
#define ALLOC_META_OFFSET ((sizeof(alloc_meta_t) + (_Alignof(max_align_t) - 1)) & ~(_Alignof(max_align_t) - 1))

typedef struct alloc_free {
    struct alloc_free *next;
} alloc_free_t;
#define MIN_ALLOC_SIZE (sizeof(alloc_free_t))

extern char **environ;
extern mt_state_t rand_state asm("__plibc_rand_state");
extern alloc_free_t *alloc_free[MAX_BUCKET + 1] asm("__plibc_alloc_free");
