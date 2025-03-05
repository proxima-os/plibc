#ifndef _FENV_H
#define _FENV_H 1

#include <bits/features.h>

#ifndef _ISOC99_SOURCE
#error "wchar.h requires C99"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define FE_TONEAREST 0
#define FE_DOWNWARD 0x2000
#define FE_UPWARD 0x4000
#define FE_TOWARDZERO 0x6000

int fegetround(void);
int fesetround(int __round);

#ifdef __cplusplus
};
#endif

#endif /* _FENV_H */
