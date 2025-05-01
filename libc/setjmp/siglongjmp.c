#include "compiler.h"
#include "setjmp.h"
#include <signal.h>
#include <stddef.h>

EXPORT void siglongjmp(sigjmp_buf env, int val) {
    if (env->__sigmask & 1) {
        sigset_t mask = env->__sigmask & ~1;
        sigprocmask(SIG_SETMASK, &mask, NULL);
    }

    longjmp(env, val);
}
