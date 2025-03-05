.section ".text.setjmp", "ax", @progbits

.balign 16
.globl setjmp
.protected setjmp
.type setjmp, @function
setjmp:
    xor %eax, %eax
    mov %rbx, 0x00(%rdi)
    mov %rbp, 0x08(%rdi)
    mov %rsp, 0x10(%rdi)
    mov %r12, 0x18(%rdi)
    mov %r13, 0x20(%rdi)
    mov %r14, 0x28(%rdi)
    mov %r15, 0x30(%rdi)
    ret
.size setjmp, . - setjmp

.section ".text.longjmp", "ax", @progbits

.balign 16
.globl longjmp
.protected longjmp
.type longjmp, @function
longjmp:
    test %esi, %esi
    jnz 1f
    xor %esi, %esi
1:  mov %esi, %eax
    mov 0x00(%rdi), %rbx
    mov 0x08(%rdi), %rbp
    mov 0x10(%rdi), %rsp
    mov 0x18(%rdi), %r12
    mov 0x20(%rdi), %r13
    mov 0x28(%rdi), %r14
    mov 0x30(%rdi), %r15
    ret
.size longjmp, . - longjmp

.section ".text.sigsetjmp", "ax", @progbits

.balign 16
.globl sigsetjmp
.protected sigsetjmp
.type sigsetjmp, @function
sigsetjmp:
    call __sigsetjmp
    jmp setjmp
.size sigsetjmp, . - sigsetjmp

