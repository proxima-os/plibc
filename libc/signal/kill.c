#include "compiler.h"
#include "signal.h"
#include <hydrogen/thread.h>
#include <sys/types.h>
#include <unistd.h>

EXPORT int kill(pid_t pid, int sig) {
    if (pid == getpid() && sig == SIGABRT) hydrogen_thread_exit(); // special case SIGABRT so STUB() works
    STUB();
}
