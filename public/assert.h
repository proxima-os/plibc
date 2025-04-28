#ifndef _ASSERT_H
#define _ASSERT_H 1

#ifdef __cplusplus
extern "C" {
#endif

/* C90 */

#ifndef NDEBUG
__attribute__((__noreturn__)) extern void __plibc_assert_fail(
        const char *__expr,
        const char *__file,
        int __line,
        const char *__func
);
#define assert(x) (__builtin_expect(!!(x), 1) ? (void)0 : __plibc_assert_fail(#x, __FILE__, __LINE__, __func__))
#else
#define assert(x) ((void)0)
#endif

#ifdef __cplusplus
};
#endif

#endif /* _ASSERT_H */
