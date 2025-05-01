.section ".text.setjmp", "ax", @progbits

.balign 16
.globl setjmp
.type setjmp, @function
setjmp:
    .cfi_startproc
    xor %esi, %esi
    jmp sigsetjmp
    .cfi_endproc
.size setjmp, . - setjmp
