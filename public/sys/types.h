#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H 1

#include <bits/features.h>
#include <bits/size_t.h>
#include <bits/ssize_t.h> /* IWYU pragma: export */
#include <bits/types.h>

#if _POSIX_C_SOURCE < 1
#error "sys/times.h requires _POSIX_C_SOURCE >= 1"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef __dev_t dev_t;
typedef __gid_t gid_t;
typedef __ino_t ino_t;
typedef __mode_t mode_t;
typedef __nlink_t nlink_t;
typedef __off_t off_t;
typedef __pid_t pid_t;
typedef __uid_t uid_t;

#ifdef __cplusplus
};
#endif

#endif /* _SYS_TYPES_H */
