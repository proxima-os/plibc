#ifndef _LIMITS_H
#define _LIMITS_H 1

#include <bits/features.h>

#ifdef __cplusplus
extern "C" {
#endif

#if _POSIX_C_SOURCE >= 1

/* ARG_MAX can be omitted if indeterminate */
/* CHILD_MAX can be omitted if indeterminate */
/* LINK_MAX can be omitted if path-dependent */
/* MAX_CANON can be omitted if path-dependent */
/* MAX_INPUT can be omitted if path-dependent */
/* NAME_MAX can be omitted if path-dependent */
#define NGROUPS_MAX 0x10000
/* OPEN_MAX can be omitted if indeterminate */
/* PATH_MAX can be omitted if path-dependent */
/* PIPE_BUF can be omitted if path-dependent */
#define SSIZE_MAX LONG_MAX
/* STREAM_MAX can be omitted if indeterminate */
/* TZNAME_MAX can be omitted if indeterminate */

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

#endif /* _POSIX_C_SOURCE >= 1 */

#ifdef __cplusplus
};
#endif

#endif /* _LIMITS_H */

/* this is outside of the inclusion guard because this header might be on the include path
 * multiple times */
#if defined(__GNUC__) && !defined(_GCC_LIMITS_H_)
#include_next <limits.h> /* IWYU pragma: keep */
#endif
