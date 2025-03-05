#ifndef _BITS_FEATURES_H
#define _BITS_FEATURES_H 1

#if defined(_POSIX_SOURCE) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 1
#endif

#if !defined(_POSIX_C_SOURCE) && !defined(__STRICT_ANSI__)
#define _POSIX_SOURCE 1
#define _POSIX_C_SOURCE 200809L
#endif

#if !defined(_ISOC99_SOURCE) && (__STDC_VERSION__ >= 199901L || _POSIX_C_SOURCE >= 200112L)
#define _ISOC99_SOURCE
#endif

#if !defined(_ISOC95_SOURCE) && (defined(_ISOC99_SOURCE) || __STDC_VERSION__ >= 199409L)
#define _ISOC95_SOURCE
#endif

#endif /* _BITS_FEATURES_H */
