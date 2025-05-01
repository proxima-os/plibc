#include "compiler.h"
#include "stdlib.h"
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

EXPORT int system(const char *string) {
    if (!string) return 1;

    int status = -1;

    struct sigaction ign_sa = {.sa_handler = SIG_IGN};
    struct sigaction old_int, old_quit;
    sigaction(SIGINT, &ign_sa, &old_int);
    sigaction(SIGQUIT, &ign_sa, &old_quit);

    sigset_t new_mask = {};
    sigset_t old_mask;
    sigaddset(&new_mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &new_mask, &old_mask);

    pid_t pid = fork();

    if (pid > 0) {
        while (waitpid(pid, &status, 0) < 0) {
            if (errno != EINTR) break;
        }
    }

    sigaction(SIGINT, &old_int, NULL);
    sigaction(SIGQUIT, &old_quit, NULL);
    sigprocmask(SIG_SETMASK, &old_mask, NULL);

    if (pid == 0) {
        execl("/bin/sh", "sh", "-c", "--", string, NULL);
        write(STDERR_FILENO, "mlibc: execl failed in system()\n", 32);
        _exit(127);
    }

    return status;
}
