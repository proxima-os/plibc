#include "compiler.h"
#include "dlfcn.h"
#include "rtld.h"

EXPORT void *dlsym(void *restrict handle, const char *restrict name) {
    return do_dlsym(handle, name);
}
