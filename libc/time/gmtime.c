#include "compiler.h"
#include "time.h"
#include "time.p.h"

EXPORT struct tm *gmtime(const time_t *timer) {
    static struct tm buffer;

    time_t secs_since_epoch = *timer;
    time_t days_since_epoch = (secs_since_epoch >= 0 ? secs_since_epoch : secs_since_epoch - 86399) / 86400;
    unsigned seconds = secs_since_epoch - days_since_epoch * 86400;
    unsigned minutes = seconds / 60;
    seconds %= 60;
    unsigned hours = minutes / 60;
    minutes %= 60;

    // https://howardhinnant.github.io/date_algorithms.html#civil_from_days
    days_since_epoch += 719468;
    time_t era = (days_since_epoch >= 0 ? days_since_epoch : days_since_epoch - 146096) / 146097;
    unsigned doe = days_since_epoch - era * 146097;
    unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    time_t year = yoe + era * 400;
    unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    unsigned mp = (5 * doy + 2) / 153;
    unsigned mday = doy - (153 * mp + 2) / 5 + 1;
    unsigned month = mp < 10 ? mp + 3 : mp - 9;

    buffer.tm_sec = seconds;
    buffer.tm_min = minutes;
    buffer.tm_hour = hours;
    buffer.tm_mday = mday;
    buffer.tm_mon = month - 1;
    buffer.tm_year = year - 1900;
    buffer.tm_wday = get_weekday(days_since_epoch);
    buffer.tm_yday = doy;
    buffer.tm_isdst = 0;

    return &buffer;
}
