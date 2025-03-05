#ifndef _ASSERT_H
#define _ASSERT_H 1

#include <bits/features.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__)) void __plibc_assert_fail(
        const char *__expr,
        const char *__file,
        int __line,
        const char *__func
);

#ifndef NDEBUG
#define assert(expr)                                                                                                   \
    (__builtin_expect(!!(expr), 1) ? (void)0 : __plibc_assert_fail(#expr, __FILE__, __LINE__, __func__))
#else
#define assert(expr) ((void)0)
#endif

#ifdef __cplusplus
};
#endif

#endif /* _ASSERT_H */
