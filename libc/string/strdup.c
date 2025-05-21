#include "compiler.h"
#include "string.h"
#include <stddef.h>
#include <stdlib.h>

EXPORT char *strdup(const char *s) {
    char *buffer = malloc(strlen(s) + 1);
    if (unlikely(!buffer)) return NULL;
    strcpy(buffer, s);
    return buffer;
}
