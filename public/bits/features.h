#ifndef _BITS_FEATURES_H
#define _BITS_FEATURES_H

#define __PLIBC_POSIX_MAX 1

#if defined(_POSIX_SOURCE) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 1
#endif

#if !defined(_DEFAULT_SOURCE) && !defined(_POSIX_C_SOURCE) && !defined(__STRICT_ANSI__)
#define _DEFAULT_SOURCE
#endif

#ifdef _DEFAULT_SOURCE

#if _POSIX_C_SOURCE < __PLIBC_POSIX_MAX
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE __PLIBC_POSIX_MAX
#endif

#endif /* defined(_DEFAULT_SOURCE) */

#if defined(_POSIX_C_SOURCE) && !defined(_POSIX_SOURCE)
#define _POSIX_SOURCE 1
#endif

#endif /* _BITS_FEATURES_H */
