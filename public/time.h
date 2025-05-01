#ifndef _TIME_H
#define _TIME_H 1

#include <bits/NULL.h>
#include <bits/clock_t.h> /* IWYU pragma: export */
#include <bits/features.h>
#include <bits/size_t.h>
#include <bits/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CLOCKS_PER_SEC 1000000

typedef __time_t time_t;

struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

clock_t clock(void);
double difftime(time_t __time1, time_t __time0);
time_t mktime(struct tm *__timeptr);
time_t time(time_t *__timer);
char *asctime(const struct tm *__timeptr);
char *ctime(const time_t *__timer);
struct tm *gmtime(const time_t *__timer);
struct tm *localtime(const time_t *__timer);
size_t strftime(
        char *__restrict __s,
        size_t __maxsize,
        const char *__restrict __format,
        const struct tm *__restrict __timeptr
);

#if _POSIX_C_SOURCE >= 1

#define CLK_TCK 1000000000ul

extern char *tzname[2];

void tzset(void);

#endif /* _POSIX_C_SOURCE >= 1 */

#ifdef __cplusplus
};
#endif

#endif /* _TIME_H */
