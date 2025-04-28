#include "compiler.h"
#include "stdio.h"
#include <limits.h>

EXPORT char *gets(char *s) {
    return fgets(s, INT_MAX, stdin);
}
