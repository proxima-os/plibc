#ifndef _STDLIB_H
#define _STDLIB_H 1

#ifndef _PLIBC_INLINE_IMPL
#define _PLIBC_INLINE_IMPL static __inline
#endif

#include <bits/features.h>

#define __need_size_t
#define __need_wchar_t
#define __need_NULL
#include <stddef.h> /* IWYU pragma: export */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int quot;
    int rem;
} div_t;

typedef struct {
    long quot;
    long rem;
} ldiv_t;

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define RAND_MAX 0x7fffffff
#define MB_CUR_MAX 4

double atof(const char *__nptr);
int atoi(const char *__nptr);
long atol(const char *__nptr);
double strtod(const char *__restrict __nptr, char **__restrict __endptr);
long strtol(const char *__restrict __nptr, char **__restrict __endptr, int __base);
unsigned long strtoul(const char *__restrict __nptr, char **__restrict __endptr, int __base);
int rand(void);
void srand(unsigned __seed);
void free(void *__ptr);
void *malloc(size_t __size);
void *realloc(void *__ptr, size_t __size);
__attribute__((__noreturn__)) void abort(void);
int atexit(void (*__func)(void));
__attribute__((__noreturn__)) void exit(int __status);
char *getenv(const char *__name);
int system(const char *__string);
void *bsearch(
        const void *__key,
        const void *__base,
        size_t __nmemb,
        size_t __size,
        int (*__compar)(const void *, const void *)
);
void qsort(void *__base, size_t __nmemb, size_t __size, int (*__compar)(const void *, const void *));
int mblen(const char *__s, size_t __n);
int mbtowc(wchar_t *__restrict __pwc, const char *__restrict __s, size_t __n);
int wctomb(char *__s, wchar_t __wchar);
size_t mbstowcs(wchar_t *__restrict __pwcs, const char *__restrict __s, size_t __n);
size_t wcstombs(char *__restrict __s, const wchar_t *__restrict __pwcs, size_t __n);

_PLIBC_INLINE_IMPL int abs(int __j) {
    return __j >= 0 ? __j : -__j;
}

_PLIBC_INLINE_IMPL div_t div(int __numer, int __denom) {
    div_t __d;
    __d.quot = __numer / __denom;
    __d.rem = __numer % __denom;
    return __d;
}

_PLIBC_INLINE_IMPL long labs(long __j) {
    return __j >= 0 ? __j : -__j;
}

_PLIBC_INLINE_IMPL ldiv_t ldiv(long __numer, long __denom) {
    ldiv_t __d;
    __d.quot = __numer / __denom;
    __d.rem = __numer % __denom;
    return __d;
}

_PLIBC_INLINE_IMPL void *calloc(size_t __nmemb, size_t __size) {
    void *__ptr = malloc(__nmemb * __size);
    if (__builtin_expect(!__ptr, 0)) return NULL;
    __builtin_memset(__ptr, 0, __nmemb * __size);
    return __ptr;
}

#ifdef __cplusplus
};
#endif

#undef _PLIBC_INLINE_IMPL

#elif defined(_PLIBC_INLINE_IMPL)
#error "_PLIBC_INLINE_IMPL defined for guarded header"
#endif /* _STDLIB_H */
