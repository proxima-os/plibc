#ifndef _GRP_H
#define _GRP_H 1

#include <bits/__gid_t.h>
#include <bits/features.h>

#ifndef _POSIX_C_SOURCE
#error "grp.h requires POSIX"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct group {
    char *gr_name;
    __gid_t gr_gid;
    char **gr_mem;
};

struct group *getgrgid(__gid_t __gid);
struct group *getgrnam(const char *__name);

#ifdef __cplusplus
};
#endif

#endif /* _GRP_H */
