#include "compiler.h"
#include "stdio.h"
#include <string.h>

EXPORT char *ctermid(char *s) {
    static char buf[L_ctermid];
    if (!s) s = buf;

    strcpy(s, "/dev/tty");
    return s;
}
