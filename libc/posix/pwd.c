#include "pwd.h"
#include "compiler.h"
#include "stub.h"
#include <stddef.h>
#include <sys/types.h>

EXPORT struct passwd *getpwuid(uid_t uid) {
    STUB();
    return NULL;
}

EXPORT struct passwd *getpwnam(const char *name) {
    STUB();
    return NULL;
}
