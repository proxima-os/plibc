#pragma once

extern _Noreturn void stub(const char *file, int line, const char *func) asm("__plibc_stub");
#define STUB() (stub(__FILE__, __LINE__, __func__))

#define EXPORT __attribute__((visibility("default")))
#define USED __attribute__((used))

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#if !defined(__clang__) && __GNUC__ >= 15
#define NONSTRING __attribute__((nonstring))
#else
#define NONSTRING
#endif
