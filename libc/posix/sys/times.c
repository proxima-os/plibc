#include "sys/times.h"
#include "stub.h"

clock_t times(struct tms *buffer) {
    STUB();
    return -1;
}
