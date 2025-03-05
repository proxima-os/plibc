#ifndef _UTIME_H
#define _UTIME_H 1

#include <bits/__time_t.h>
#include <bits/features.h>

#ifndef _POSIX_C_SOURCE
#error "utime.h requires POSIX"
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
