#ifndef _LIMITS_H
#define _LIMITS_H 1

#include <bits/features.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MB_LEN_MAX 4 /* We only support UTF-8 */

#ifdef _POSIX_C_SOURCE

#define _POSIX_ARG_MAX 4096
#define _POSIX_CHILD_MAX 6
#define _POSIX_LINK_MAX 8
#define _POSIX_MAX_CANON 255
#define _POSIX_MAX_INPUT 255
#define _POSIX_NAME_MAX 14
#define _POSIX_NGROUPS_MAX 0
#define _POSIX_OPEN_MAX 16
#define _POSIX_PATH_MAX 255
#define _POSIX_PIPE_BUF 512
#define _POSIX_SSIZE_MAX 32767
#define _POSIX_STREAM_MAX 8
#define _POSIX_TZNAME_MAX 3

#define NGROUPS_MAX _POSIX_NGROUPS_MAX

#define SSIZE_MAX 0x7fffffffffffffff

#endif /* defined(_POSIX_C_SOURCE) */

#ifdef __cplusplus
};
#endif

#endif /* _LIMITS_H */

/* Include this from outside the guard in case this limits.h is on the include path multiple times */
#include_next <limits.h> /* IWYU pragma: export */
