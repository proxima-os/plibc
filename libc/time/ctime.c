#include "compiler.h"
#include "time.h"

EXPORT char *ctime(const time_t *timer) {
    return asctime(localtime(timer));
}
