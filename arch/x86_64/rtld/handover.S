.section ".text.rtld_handover", "ax", @progbits

.balign 16
.globl rtld_handover
.hidden rtld_handover
.type rtld_handover, @function
rtld_handover:
    .cfi_startproc
    mov %rsi, %rsp
    .cfi_undefined rip
    .cfi_undefined rsp
    jmp *%rdi
    .cfi_endproc
.size rtld_handover, . - rtld_handover
