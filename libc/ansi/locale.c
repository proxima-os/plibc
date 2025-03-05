#include "locale.h"
#include <limits.h>
#include <string.h>

// TODO: Locale support

static struct lconv c_lconv = {
        .decimal_point = ".",
        .thousands_sep = "",
        .grouping = "",
        .mon_decimal_point = "",
        .mon_thousands_sep = "",
        .mon_grouping = "",
        .positive_sign = "",
        .negative_sign = "",
        .currency_symbol = "",
        .frac_digits = CHAR_MAX,
        .p_cs_precedes = CHAR_MAX,
        .n_cs_precedes = CHAR_MAX,
        .p_sep_by_space = CHAR_MAX,
        .n_sep_by_space = CHAR_MAX,
        .p_sign_posn = CHAR_MAX,
        .n_sign_posn = CHAR_MAX,
        .int_curr_symbol = "",
        .int_frac_digits = CHAR_MAX,
};

char *setlocale(int category, const char *locale) {
    if (!locale) return "C";
    if (!strcmp(locale, "C") || !strcmp(locale, "POSIX")) return "C";
    return NULL;
}

struct lconv *localeconv(void) {
    return &c_lconv;
}
