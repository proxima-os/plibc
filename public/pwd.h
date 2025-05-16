#ifndef _PWD_H
#define _PWD_H

#include <bits/features.h>
#include <bits/types.h>

#if _POSIX_C_SOURCE < 1
#error "pwd.h requires _POSIX_C_SOURCE >= 1"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct passwd {
    char *pw_name;
    char *pw_dir;
    char *pw_shell;
    __uid_t pw_uid;
    __gid_t pw_gid;
};

struct passwd *getpwnam(const char *__name);
struct passwd *getpwuid(__uid_t __uid);

#ifdef __cplusplus
};
#endif

#endif /* _PWD_H */
