#ifndef _SYS_AUXV_H
#define _SYS_AUXV_H 1

#include <bits/features.h>

#ifndef _PROXIMA_SOURCE
#error "sys/auxv.h requires _PROXIMA_SOURCE"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define AT_NULL 0
#define AT_SYSINFO_EHDR 33

unsigned long getauxval(unsigned long __type);

#ifdef __cplusplus
};
#endif

#endif /* _SYS_AUXV_H */
