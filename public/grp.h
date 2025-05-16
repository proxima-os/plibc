#ifndef _GRP_H
#define _GRP_H

#include <bits/features.h>
#include <bits/types.h>

#if _POSIX_C_SOURCE < 1
#error "grp.h requires _POSIX_C_SOURCE >= 1"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct group {
    char *gr_name;
    char **gr_mem;
    __gid_t gr_gid;
};

struct group *getgrgid(__gid_t __gid);
struct group *getgrnam(const char *__name);

#ifdef __cplusplus
};
#endif

#endif /* _GRP_H */
