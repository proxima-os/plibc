#include "sys/utsname.h"
#include "compiler.h"
#include "stub.h"

EXPORT int uname(struct utsname *name) {
    STUB();
    return -1;
}
