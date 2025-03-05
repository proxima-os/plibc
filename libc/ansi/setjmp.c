#include "setjmp.h"
#include <signal.h>
#include <stddef.h>
#include <stdint.h>

void __sigsetjmp(sigjmp_buf env, int savemask) {
    if (savemask) {
        sigprocmask(SIG_BLOCK, NULL, &env[0].__mask);
    } else {
        env[0].__mask = (sigset_t)-1;
    }
}

void siglongjmp(sigjmp_buf env, int val) {
    if (env[0].__mask != (sigset_t)-1) {
        sigprocmask(SIG_SETMASK, &env[0].__mask, NULL);
    }

    longjmp(env[0].__buf, val);
}
