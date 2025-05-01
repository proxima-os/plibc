#ifndef _SYS_TIMES_H
#define _SYS_TIMES_H 1

#include <bits/features.h>
#include <bits/clock_t.h>

#if _POSIX_C_SOURCE < 1
#error "sys/times.h requires _POSIX_C_SOURCE >= 1"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct tms {
    clock_t tms_utime;
    clock_t tms_stime;
    clock_t tms_cutime;
    clock_t tms_cstime;
};

clock_t times(struct tms *__buffer);

#ifdef __cplusplus
};
#endif

#endif /* _SYS_TIMES_H */
