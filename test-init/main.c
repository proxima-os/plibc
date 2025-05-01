#include <hydrogen/init.h>
#include <hydrogen/log.h>
#include <stdio.h>
#include <sys/auxv.h>
#include <unistd.h>

__attribute__((constructor)) static void ctor_test(void) {
    hydrogen_init_info_t *init = (hydrogen_init_info_t *)getauxval(HYDROGEN_AT_INIT_INFO);
    hydrogen_log_write(init->log_handle, "ctor!\n", 6);
}

__attribute__((destructor)) static void dtor_test(void) {
    hydrogen_init_info_t *init = (hydrogen_init_info_t *)getauxval(HYDROGEN_AT_INIT_INFO);
    hydrogen_log_write(init->log_handle, "dtor!\n", 6);
}

int main(int argc, char *argv[]) {
    printf("Hello from test-init! argc = %d, argv[0] = %s\n", argc, argv[0]);

    if (fork() == 0) {
        printf("Hello from forked process!\n");
    }

    return 0;
}
