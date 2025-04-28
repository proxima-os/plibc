#pragma once

#include <time.h>

static inline int get_weekday(time_t days_since_epoch) {
    // https://howardhinnant.github.io/date_algorithms.html#weekday_from_days
    return days_since_epoch >= -4 ? (days_since_epoch + 4) % 7 : (days_since_epoch + 5) % 7 + 6;
}
