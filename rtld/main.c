#include <elf.h>
#include <stdio.h>
#include <sys/auxv.h>

int main(void) {
    printf("Hello, World 0x%lx!\n", getauxval(AT_ENTRY));
}
