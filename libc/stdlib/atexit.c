#include "compiler.h"
#include "cxa.h"
#include "stdlib.h"

EXPORT int atexit(void (*func)(void)) {
    return __cxa_atexit((void (*)(void *))func, NULL, NULL);
}
