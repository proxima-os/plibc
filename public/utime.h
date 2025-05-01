#ifndef _UTIME_H
#define _UTIME_H 1

#include <bits/features.h>
#include <bits/types.h>

#if _POSIX_C_SOURCE < 1
#error "utime.h requires _POSIX_C_SOURCE >= 1"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct utimbuf {
    __time_t actime;
    __time_t modtime;
};

int utime(const char *__path, const struct utimbuf *__times);

#ifdef __cplusplus
};
#endif

#endif /* _UTIME_H */
