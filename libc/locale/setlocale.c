#include "compiler.h"
#include "locale.h"
#include <errno.h>
#include <string.h>

EXPORT char *setlocale(int category, const char *locale) {
    if (locale != NULL && locale[0] != 0 && strcmp(locale, "C") && strcmp(locale, "POSIX")) {
        errno = ENOENT;
        return NULL;
    }

    switch (category) {
    case LC_ALL:
    case LC_COLLATE:
    case LC_CTYPE:
    case LC_MONETARY:
    case LC_NUMERIC:
    case LC_TIME: return "C";
    default: errno = EINVAL; return NULL;
    }
}
