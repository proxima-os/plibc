#ifndef _SYS_TIMES_H
#define _SYS_TIMES_H 1

#include <bits/clock_t.h> /* IWYU pragma: export */
#include <bits/features.h>

#ifndef _POSIX_C_SOURCE
#error "sys/times.h requires POSIX"
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
