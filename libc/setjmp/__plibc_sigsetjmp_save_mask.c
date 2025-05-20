#include <setjmp.h>
#include <signal.h>
#include <stddef.h>

__attribute__((used)) void __plibc_sigsetjmp_save_mask(sigjmp_buf buf, int should_save) {
    if (should_save) {
        sigprocmask(SIG_BLOCK, NULL, &buf->__sigmask);
        buf->__sigmask |= 1;
    } else {
        buf->__sigmask = 0;
    }
}
