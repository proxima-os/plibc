#include "compiler.h"
#include "dlfcn.h"
#include "rtld.h"

EXPORT int dlclose(void *handle) {
    return do_dlclose(handle);
}
