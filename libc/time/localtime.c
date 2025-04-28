#include "compiler.h"
#include "time.h"

EXPORT struct tm *localtime(const time_t *timer) {
    return gmtime(timer);
}
