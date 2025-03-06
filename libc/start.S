.section ".text.__plibc_start", "ax", @progbits

.weak _DYNAMIC
.weak _GLOBAL_OFFSET_TABLE_

.globl __plibc_start
.protected __plibc_start
.type __plibc_start, @function
__plibc_start:
    push %rdi
    push %rsi
    push %rdx

    lea _DYNAMIC(%rip), %rsi
    lea _GLOBAL_OFFSET_TABLE_(%rip), %rdx
    call __plibc_rtld_init

    pop %rdx
    pop %rsi
    pop %rdi
    jmp __plibc_main
.size __plibc_start, . - __plibc_start
