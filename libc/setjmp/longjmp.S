.section ".text.longjmp", "ax", @progbits

.balign 16
.globl longjmp
.type longjmp, @function
longjmp:
    .cfi_startproc
    mov %esi, %eax
    test %esi, %esi
    jz 1f
    inc %eax
1:
    mov 0x00(%rdi), %rbx
    mov 0x08(%rdi), %rbp
    mov 0x10(%rdi), %r12
    mov 0x18(%rdi), %r13
    mov 0x20(%rdi), %r14
    mov 0x28(%rdi), %r15
    mov 0x30(%rdi), %rsp
    jmp *0x38(%rdi)
    .cfi_endproc
.size longjmp, . - longjmp
