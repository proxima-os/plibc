#ifndef _SYS_UTSNAME_H
#define _SYS_UTSNAME_H 1

#include <bits/features.h>

#if _POSIX_C_SOURCE < 1
#error "sys/utsname.h requires _POSIX_C_SOURCE >= 1"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct utsname {
    char machine[64];
    char nodename[64];
    char release[64];
    char sysname[64];
    char version[64];
};

int uname(struct utsname *__name);

#ifdef __cplusplus
};
#endif

#endif /* _SYS_UTSNAME_H */
