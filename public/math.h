#ifndef _MATH_H
#define _MATH_H 1

#include <bits/features.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HUGE_VAL (__builtin_huge_val())

double acos(double __x);
double asin(double __x);
double atan(double __x);
double atan2(double __y, double __x);
double ceil(double __x);
double cos(double __x);
double cosh(double __x);
double exp(double __x);
double fabs(double __x);
double floor(double __x);
double fmod(double __x, double __y);
double frexp(double __x, int *__exp);
double ldexp(double __x, int __exp);
double log(double __x);
double log10(double __x);
double modf(double __x, double *__iptr);
double pow(double __x, double __y);
double sin(double __x);
double sinh(double __x);
double sqrt(double __x);
double tan(double __x);
double tanh(double __x);

#if defined(_PROXIMA_SOURCE)

#define isinf __builtin_isinf
#define isnan __builtin_isnan

typedef double double_t;

double expm1(double __x);
double scalbn(double __x, int __n);

#endif /* defined(_PROXIMA_SOURCE) */

#ifdef __cplusplus
};
#endif

#endif /* _MATH_H */
