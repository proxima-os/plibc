#include <stdio.h>
#include <sys/auxv.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("Hello from test-init! argc = %d, argv[0] = %s\n", argc, argv[0]);

    if (fork() == 0) {
        printf("Hello from forked process!\n");
    }

    return 0;
}
