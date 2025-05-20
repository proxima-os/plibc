.section ".text.sigsetjmp", "ax", @progbits

.balign 16
.globl sigsetjmp
.type sigsetjmp, @function
sigsetjmp:
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

    sub $8, %rsp # align stack for call
    call __plibc_sigsetjmp_save_mask
    add $8, %rsp

    xor %eax, %eax
    ret
.size sigsetjmp, . - sigsetjmp

.section ".note.GNU-stack", "", @progbits
