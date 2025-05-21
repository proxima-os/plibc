#ifndef _BITS_FEATURES_H
#define _BITS_FEATURES_H

#if defined(_POSIX_SOURCE) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 1
#endif

#if !defined(_DEFAULT_SOURCE) && !defined(_POSIX_C_SOURCE) && !defined(__STRICT_ANSI__)
#define _DEFAULT_SOURCE
#endif

#if defined(_DEFAULT_SOURCE) || defined(_PROXIMA_SOURCE)
#if _POSIX_C_SOURCE < 1
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 1
#endif
#endif /* defined(_DEFAULT_SOURCE) || defined(_PROXIMA_SOURCE) */

#if defined(_POSIX_C_SOURCE) && !defined(_POSIX_SOURCE)
#define _POSIX_SOURCE 1
#endif

#endif /* _BITS_FEATURES_H */
