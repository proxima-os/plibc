#include "compiler.h"
#include "dlfcn.h"
#include "rtld.h"

__attribute__((noinline)) EXPORT void *dlopen(const char *file, int mode) {
    return do_dlopen(file, mode, __builtin_extract_return_addr(__builtin_return_address(0)));
}
