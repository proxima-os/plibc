#include "grp.h"
#include "stub.h"
#include <stddef.h>
#include <sys/types.h>

struct group *getgrgid(gid_t __gid) {
    STUB();
    return NULL;
}

struct group *getgrnam(const char *name) {
    STUB();
    return NULL;
}
