#ifndef _WCHAR_H
#define _WCHAR_H 1

#include <bits/features.h>
#include <bits/FILE.h>

#ifndef _ISOC95_SOURCE
#error "wchar.h requires C95"
#endif

#define __need_wchar_t
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int wint_t;

#define WEOF (-1)

wint_t fputwc(wchar_t __c, FILE *__stream);
wint_t putwc(wchar_t __c, FILE *__stream);

#ifdef __cplusplus
};
#endif

#endif /* _WCHAR_H */
