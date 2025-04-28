.section ".text.setjmp", "ax", @progbits

.balign 16
.globl setjmp
.type setjmp, @function
setjmp:
    .cfi_startproc
    mov %rbx, 0x00(%rdi)
    mov %rbp, 0x08(%rdi)
    mov %r12, 0x10(%rdi)
    mov %r13, 0x18(%rdi)
    mov %r14, 0x20(%rdi)
    mov %r15, 0x28(%rdi)

    lea 8(%rsp), %rax
    mov %rax, 0x30(%rdi)
    mov (%rsp), %rax
    mov %rax, 0x38(%rdi)

    xor %eax, %eax
    ret
    .cfi_endproc
.size setjmp, . - setjmp
