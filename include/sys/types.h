#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H 1

#include <bits/__dev_t.h>
#include <bits/__gid_t.h>
#include <bits/__ino_t.h>
#include <bits/__mode_t.h>
#include <bits/__nlink_t.h>
#include <bits/__off_t.h>
#include <bits/__pid_t.h>
#include <bits/__uid_t.h>
#include <bits/features.h>
#include <bits/off_t.h>   /* IWYU pragma: export */
#include <bits/ssize_t.h> /* IWYU pragma: export */

#define __need_size_t
#include <stddef.h> /* IWYU pragma: export */

#ifndef _POSIX_C_SOURCE
#error "sys/types.h requires POSIX"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef __dev_t dev_t;
typedef __gid_t gid_t;
typedef __ino_t ino_t;
typedef __mode_t mode_t;
typedef __nlink_t nlink_t;
typedef __pid_t pid_t;
typedef __uid_t uid_t;

#ifdef __cplusplus
};
#endif

#endif /* _SYS_TYPES_H */
