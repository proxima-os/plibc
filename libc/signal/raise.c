#include "compiler.h"
#include "signal.h"
#include <unistd.h>

EXPORT int raise(int sig) {
    return kill(getpid(), sig);
}
