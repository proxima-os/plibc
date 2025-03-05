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
double cos(double __x);
double sin(double __x);
double tan(double __x);
double cosh(double __x);
double sinh(double __x);
double tanh(double __x);
double exp(double __x);
double frexp(double __value, int *__exp);
double ldexp(double __value, int __exp);
double log(double __x);
double log10(double __x);
double modf(double __value, double *__iptr);
double pow(double __x, double __y);
double sqrt(double __x);
double ceil(double __x);
double fabs(double __x);
double floor(double __x);
double fmod(double __x, double __y);

#ifdef _ISOC99_SOURCE

#define HUGE_VALF (__builtin_huge_valf())
#define HUGE_VALL (__builtin_huge_vall())
#define INFINITY (__builtin_inff())
#define NAN (__builtin_nanf(""))

#define FP_NORMAL 0
#define FP_SUBNORMAL 1
#define FP_ZERO 2
#define FP_INFINITE 3
#define FP_NAN 4

#ifdef __FP_FAST_FMA
#define FP_FAST_FMA 1
#endif

#ifdef __FP_FAST_FMAF
#define FP_FAST_FMAF 1
#endif

#ifdef __FP_FAST_FMAL
#define FP_FAST_FMAL 1
#endif

#define FP_ILOGB0 (-__INT_MAX__ - 1)
#define FP_ILOGBNAN (-__INT_MAX__ - 1)

#define MATH_ERRNO 1
#define MATH_ERREXCEPT 2
#define math_errhandling MATH_ERREXCEPT

#define fpclassify(x) (__builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, (x)))
#define isfinite(x) (__builtin_isfinite(x))
#define isinf(x) (__builtin_isinf_sign(x))
#define isnan(x) (__builtin_isnan(x))
#define isnormal(x) (__builtin_isnormal(x))
#define signbit(x) (__builtin_signbit(x))
#define isgreater(x, y) (__builtin_isgreater((x), (y)))
#define isgreaterequal(x, y) (__builtin_isgreaterequal((x), (y)))
#define isless(x, y) (__builtin_isless((x), (y)))
#define islessequal(x, y) (__builtin_islessequal((x), (y)))
#define islessgreater(x, y) (__builtin_islessgreater((x), (y)))
#define isunordered(x, y) (__builtin_isunordered((x), (y)))

typedef float float_t;
typedef double double_t;

#define EFFUNCF2F(name, p0)                                                                                            \
    float name##f(float p0);                                                                                           \
    long double name##l(long double p0)

#define EFFUNCO2F(name, p0)                                                                                            \
    float name##f(p0);                                                                                                 \
    long double name##l(p0)

#define EFFUNCF2O(name, p0, ret)                                                                                       \
    ret name##f(float p0);                                                                                             \
    ret name##l(long double p0)

#define EFFUNCFF2F(name, p0, p1)                                                                                       \
    float name##f(float p0, float p1);                                                                                 \
    long double name##l(long double p0, long double p1)

#define EFFUNCFO2F(name, p0, p1)                                                                                       \
    float name##f(float p0, p1);                                                                                       \
    long double name##l(long double p0, p1)

#define EFFUNCFFF2F(name, p0, p1, p2)                                                                                  \
    float name##f(float p0, float p1, float p2);                                                                       \
    long double name##l(long double p0, long double p1, long double p2)

#define EFFUNCFFO2F(name, p0, p1, p2)                                                                                  \
    float name##f(float p0, float p1, p2);                                                                             \
    long double name##l(long double p0, long double p1, p2)

#define NFFUNCF2F(name, p0)                                                                                            \
    double name(double p0);                                                                                            \
    EFFUNCF2F(name, p0)

#define NFFUNCO2F(name, p0)                                                                                            \
    double name(p0);                                                                                                   \
    EFFUNCO2F(name, p0)

#define NFFUNCF2O(name, p0, ret)                                                                                       \
    ret name(double p0);                                                                                               \
    EFFUNCF2O(name, p0, ret)

#define NFFUNCFF2F(name, p0, p1)                                                                                       \
    double name(double p0, double p1);                                                                                 \
    EFFUNCFF2F(name, p0, p1)

#define NFFUNCFO2F(name, p0, p1)                                                                                       \
    double name(double p0, p1);                                                                                        \
    EFFUNCFO2F(name, p0, p1)

#define NFFUNCFFF2F(name, p0, p1, p2)                                                                                  \
    double name(double p0, double p1, double p2);                                                                      \
    EFFUNCFFF2F(name, p0, p1, p2)

#define NFFUNCFFO2F(name, p0, p1, p2)                                                                                  \
    double name(double p0, double p1, p2);                                                                             \
    EFFUNCFFO2F(name, p0, p1, p2)

EFFUNCF2F(acos, __x);
EFFUNCF2F(asin, __x);
EFFUNCF2F(atan, __x);
EFFUNCF2F(asin, __x);
EFFUNCFF2F(atan2, __y, __x);
EFFUNCF2F(cos, __x);
EFFUNCF2F(sin, __x);
EFFUNCF2F(tan, __x);
NFFUNCF2F(acosh, __x);
NFFUNCF2F(asinh, __x);
NFFUNCF2F(atanh, __x);
EFFUNCF2F(cosh, __x);
EFFUNCF2F(sinh, __x);
EFFUNCF2F(tanh, __x);
EFFUNCF2F(exp, __x);
NFFUNCF2F(exp2, __x);
NFFUNCF2F(expm1, __x);
EFFUNCFO2F(frexp, __value, int *__exp);
NFFUNCF2O(ilogb, __x, int);
EFFUNCFO2F(ldexp, __x, int __exp);
EFFUNCF2F(log, __x);
EFFUNCF2F(log10, __x);
NFFUNCF2F(log1p, __x);
NFFUNCF2F(log2, __x);
NFFUNCF2F(logb, __x);
EFFUNCFF2F(modf, __value, *__iptr);
NFFUNCFO2F(scalbn, __x, int __n);
NFFUNCFO2F(scalbln, __x, long __n);
NFFUNCF2F(cbrt, __x);
EFFUNCF2F(fabs, __x);
NFFUNCFF2F(hypot, __x, __y);
EFFUNCFF2F(pow, __x, __y);
EFFUNCF2F(sqrt, __x);
NFFUNCF2F(erf, __x);
NFFUNCF2F(erfc, __x);
NFFUNCF2F(lgamma, __x);
NFFUNCF2F(tgamma, __x);
EFFUNCF2F(ceil, __x);
EFFUNCF2F(floor, __x);
NFFUNCF2F(nearbyint, __x);
NFFUNCF2F(rint, __x);
NFFUNCF2O(lrint, __x, long);
NFFUNCF2O(llrint, __x, long long);
NFFUNCF2F(round, __x);
NFFUNCF2O(lround, __x, long);
NFFUNCF2O(llround, __x, long long);
NFFUNCF2F(trunc, __x);
EFFUNCFF2F(fmod, __x, __y);
NFFUNCFF2F(remainder, __x, __y);
NFFUNCFFO2F(remquo, __x, __y, int *__quo);
NFFUNCFF2F(copysign, __x, __y);
NFFUNCO2F(nan, const char *__tagp);
NFFUNCFF2F(nextafter, __x, __y);
NFFUNCFO2F(nexttoward, __x, long double __y);
NFFUNCFF2F(fdim, __x, __y);
NFFUNCFF2F(fmax, __x, __y);
NFFUNCFF2F(fmin, __x, __y);
NFFUNCFFF2F(fma, __x, __y, __z);

#undef EFFUNCF2F
#undef EFFUNCO2F
#undef EFFUNCF2O
#undef EFFUNCFF2F
#undef EFFUNCFO2F
#undef EFFUNCFFF2F
#undef EFFUNCFFO2F
#undef NFFUNCF2F
#undef NFFUNCO2F
#undef NFFUNCF2O
#undef NFFUNCFF2F
#undef NFFUNCFO2F
#undef NFFUNCFFF2F
#undef NFFUNCFFO2F

#endif /* defined(_ISOC99_SOURCE) */

#if _POSIX_C_SOURCE >= 200112L
#define M_E 2.7182818284590452354
#define M_LOG2E 1.4426950408889634074
#define M_LOG10E 0.43429448190325182765
#define M_LN2 0.69314718055994530942
#define M_LN10 2.30258509299404568402
#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923
#define M_PI_4 0.78539816339744830962
#define M_1_PI 0.31830988618379067154
#define M_2_PI 0.63661977236758134308
#define M_2_SQRTPI 1.12837916709551257390
#define M_SQRT2 1.41421356237309504880
#define M_SQRT1_2 0.70710678118654752440
#endif /* _POSIX_C_SOURCE */

#ifdef __cplusplus
};
#endif

#endif /* _MATH_H */
