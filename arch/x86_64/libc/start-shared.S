.section ".text.__plibc_start", "ax", @progbits

.globl __plibc_start
.type __plibc_start, @function
__plibc_start:
    jmp __plibc_main
.size __plibc_start, . - __plibc_start

.section ".note.GNU-stack", "", @progbits
