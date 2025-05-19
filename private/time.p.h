#pragma once

#include <time.h>

static inline int get_weekday(time_t days_since_epoch) {
    // https://howardhinnant.github.io/date_algorithms.html#weekday_from_days
    return days_since_epoch >= -4 ? (days_since_epoch + 4) % 7 : (days_since_epoch + 5) % 7 + 6;
}

static inline void timestamp_to_timespec(struct __timespec *out, __int128_t timestamp) {
    if (timestamp >= 0) {
        out->tv_sec = timestamp / 1000000000;
        out->tv_nsec = timestamp % 1000000000;
    } else {
        out->tv_sec = (timestamp - 999999999) / 1000000000;
        out->tv_nsec = timestamp - (out->tv_sec * 1000000000);
    }
}
