#include "compiler.h"
#include "stdio.h"
#include "tmpnam.h"

EXPORT char *tmpnam(char *s) {
    static tmpnam_t ctx;
    static char buf[L_tmpnam];

    if (!s) s = buf;
    tmpnam_gen(&ctx, s);
    return s;
}
