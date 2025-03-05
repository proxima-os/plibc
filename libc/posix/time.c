#include "time.h"
#include "compiler.h"
#include "stub.h"

char *tzname[2];

EXPORT void tzset(void) {
    STUB();
}
