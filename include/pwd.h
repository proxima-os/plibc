#ifndef _PWD_H
#define _PWD_H 1

#include <bits/__gid_t.h>
#include <bits/__uid_t.h>
#include <bits/features.h>

#ifndef _POSIX_C_SOURCE
#error "pwd.h requires POSIX"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct passwd {
    char *pw_name;
    __uid_t pw_uid;
    __gid_t pw_gid;
    char *pw_dir;
    char *pw_shell;
};

struct passwd *getpwuid(__uid_t uid);
struct passwd *getpwnam(const char *__name);

#ifdef __cplusplus
};
#endif

#endif /* _PWD_H */
