#include "compiler.h"
#include "stdio.h"
#include <errno.h> /* IWYU pragma: keep */
#include <string.h>

EXPORT void perror(const char *s) {
    if (s != NULL && s[0] != 0) fprintf(stderr, "%s: %s\n", s, strerror(errno));
    else fprintf(stderr, "%s\n", strerror(errno));
}
