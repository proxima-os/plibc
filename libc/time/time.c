#include "time.h"
#include "compiler.h"
#include <hydrogen/time.h>

EXPORT time_t time(time_t *timer) {
    time_t val = hydrogen_get_real_time() / 1000000000;
    if (timer) *timer = val;
    return val;
}
