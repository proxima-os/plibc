#include "compiler.h"
#include "time.h"

EXPORT double difftime(time_t time1, time_t time0) {
    return time1 - time0;
}
