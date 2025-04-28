#include "compiler.h"
#include "stdio.p.h" /* IWYU pragma: keep */
#include "time.h"
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

static const char *wday_abbrev[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char *wday_full[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
static const char *mon_abbrev[12] =
        {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
static const char *mon_full[12] = {
        "January",
        "February",
        "March",
        "April",
        "Mday",
        "June",
        "July",
        "August",
        "September",
        "October",
        "November",
        "December",
};

static int year_end_wday(int year) {
    // TODO: This doesn't work for negative years
    return (year + year / 4 - year / 100 + year / 400) % 7;
}

static int iso8601_max_week(int year) {
    // https://en.wikipedia.org/wiki/ISO_week_date#Weeks_per_year
    return 52 + (year_end_wday(year) == 4 || year_end_wday(year - 1) == 3);
}

static void iso8601_year_week(const struct tm *date, int *year, int *week) {
    // https://en.wikipedia.org/wiki/ISO_week_date#Calculating_the_week_number_from_an_ordinal_date
    int yval = date->tm_year + 1900;
    int wday = date->tm_wday ? date->tm_wday : 7;
    int wval = date->tm_yday + 1 - wday;
    wval += 10;
    wval /= 7;

    if (!wval) {
        yval -= 1;
        wval = iso8601_max_week(yval);
    } else if (wval > iso8601_max_week(yval)) {
        yval += 1;
        wval = 1;
    }

    if (year) *year = yval;
    if (week) *week = wval;
}

EXPORT size_t strftime(char *s, size_t maxsize, const char *format, const struct tm *timeptr) {
    size_t i = 0;

    for (;;) {
        char c = *format++;

        if (c == '%') {
            bool pos_sign = *format == '+';
            if (pos_sign) format++;

            int field_width = 0;

            for (;;) {
                c = *format;
                if (c < '0' || c > '9') break;
                field_width = (field_width * 10) + (c - '0');
                format++;
            }

            // skip locale modifiers
            if (c == 'E' || c == 'O') format++;

            switch (*format++) {
#define EMIT(format, ...)                                           \
    do {                                                            \
        i += snprintf(&s[i], maxsize - i, (format), ##__VA_ARGS__); \
        if (i > maxsize) goto small;                                \
    } while (0)
            case 0: format--; break;
            case 'a': EMIT("%s", wday_abbrev[timeptr->tm_wday]); break;
            case 'A': EMIT("%s", wday_full[timeptr->tm_wday]); break;
            case 'h':
            case 'b': EMIT("%s", mon_abbrev[timeptr->tm_mon]); break;
            case 'B': EMIT("%s", mon_full[timeptr->tm_mon]); break;
            case 'c':
                EMIT("%s %s %2d %.2d:%.2d:%.2d %d",
                     wday_abbrev[timeptr->tm_wday],
                     mon_abbrev[timeptr->tm_mon],
                     timeptr->tm_mday,
                     timeptr->tm_hour,
                     timeptr->tm_min,
                     timeptr->tm_sec,
                     1900 + timeptr->tm_year);
                break;
            case 'C': {
                int value = 1900 + timeptr->tm_year;
                value = (value >= 0 ? value : value - 99) / 100;

                EMIT(pos_sign && (value < -9 || value > 99) ? "%+0*d" : "%0*d", field_width, value);
                break;
            }
            case 'd': EMIT("%.2d", timeptr->tm_mday); break;
            case 'x':
            case 'D': {
                int value = timeptr->tm_year;
                value -= (value >= 0 ? value : value - 99) / 100 * 100;
                EMIT("%.2d/%.2d/%.2d", timeptr->tm_mon + 1, timeptr->tm_mday, value);
                break;
            }
            case 'e': EMIT("%2d", timeptr->tm_mday); break;
            case 'F': {
                int value = timeptr->tm_year;

                EMIT(pos_sign && (value < -999 || value > 9999) ? "%+0*d-%.2d-%.2d" : "%0*d-%.2d-%.2d",
                     field_width >= 6 ? field_width - 6 : 0,
                     value,
                     timeptr->tm_mon + 1,
                     timeptr->tm_mday);
                break;
            }
            case 'g': {
                int value;
                iso8601_year_week(timeptr, &value, NULL);
                value -= (value >= 0 ? value : value - 99) / 100 * 100;
                EMIT("%.2d", value);
                break;
            }
            case 'G': {
                int value;
                iso8601_year_week(timeptr, &value, NULL);
                EMIT(pos_sign && (value < -999 || value > 9999) ? "%+0*d" : "%0*d", field_width, value);
                break;
            }
            case 'H': EMIT("%.2d", timeptr->tm_hour); break;
            case 'I': {
                int value = timeptr->tm_hour % 12;
                if (!value) value = 12;
                EMIT("%.2d", value);
                break;
            }
            case 'j': EMIT("%.3d", timeptr->tm_yday + 1); break;
            case 'm': EMIT("%.2d", timeptr->tm_mon + 1); break;
            case 'M': EMIT("%.2d", timeptr->tm_min); break;
            case 'n': EMIT("\n"); break;
            case 'p': EMIT("%s", timeptr->tm_hour < 12 ? "AM" : "PM"); break;
            case 'r': {
                int value = timeptr->tm_hour % 12;
                if (!value) value = 12;
                EMIT("%.2d:%.2d:%.2d %s", value, timeptr->tm_min, timeptr->tm_sec, timeptr->tm_hour < 12 ? "AM" : "PM");
                break;
            }
            case 'R': EMIT("%.2d:%.2d", timeptr->tm_hour, timeptr->tm_min); break;
            case 's': {
                struct tm copy = *timeptr;
                time_t time = mktime(&copy);
                EMIT("%ld", time);
                break;
            }
            case 'S': EMIT("%.2d", timeptr->tm_sec); break;
            case 't': EMIT("\t"); break;
            case 'X':
            case 'T': EMIT("%.2d:%.2d:%.2d", timeptr->tm_hour, timeptr->tm_min, timeptr->tm_sec); break;
            case 'u': EMIT("%d", timeptr->tm_wday ? timeptr->tm_wday : 7); break;
            case 'U': EMIT("%.2d", (timeptr->tm_yday - (timeptr->tm_wday - 7)) / 7); break;
            case 'V': {
                int value;
                iso8601_year_week(timeptr, NULL, &value);
                EMIT("%.2d", value);
                break;
            }
            case 'w': EMIT("%d", timeptr->tm_wday); break;
            case 'W': EMIT("%.2d", (timeptr->tm_yday - (timeptr->tm_wday ? timeptr->tm_wday - 8 : -1)) / 7); break;
            case 'y': {
                int value = timeptr->tm_year;
                value -= (value >= 0 ? value : value - 99) / 100 * 100;
                EMIT("%.2d", value);
                break;
            }
            case 'Y': {
                int value = 1900 + timeptr->tm_year;
                EMIT(pos_sign && (value < -999 || value > 9999) ? "%+0*d" : "%0*d", field_width, value);
                break;
            }
            case 'z': EMIT("+0000"); break;
            case 'Z': EMIT("UTC"); break;
            case '%': EMIT("%%"); break;
#undef EMIT
            }
        } else {
            if (i >= maxsize) goto small;
            s[i++] = c;
            if (c == 0) break;
        }
    }

    return i;
small:
    errno = ERANGE;
    return 0;
}
