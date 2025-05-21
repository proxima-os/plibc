#include "dlfcn.h"
#include "compiler.h"
#include "rtld.h"

EXPORT char *dlerror(void) {
    return do_dlerror();
}
