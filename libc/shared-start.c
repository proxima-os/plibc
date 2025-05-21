#include "start.h"

__attribute__((constructor)) void plibc_shared_init(int argc, char *argv[], char *envp[]) {
    __plibc_init(argc, argv, envp);
}
