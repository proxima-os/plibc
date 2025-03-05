#ifndef _SYS_UTSNAME_H
#define _SYS_UTSNAME_H 1

#include <bits/features.h>

#ifndef _POSIX_C_SOURCE
#error "sys/utsname.h requires POSIX"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct utsname {
    char sysname[65];
    char nodename[65];
    char release[65];
    char version[65];
    char machine[65];
};

int uname(struct utsname *__name);

#ifdef __cplusplus
};
#endif

#endif /* _SYS_UTSNAME_H */
