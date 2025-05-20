#include "compiler.h"
#include "unistd.h"
#include <stdlib.h>
#include <sys/types.h>

EXPORT char *getlogin(void) {
    return getenv("LOGNAME");
}
