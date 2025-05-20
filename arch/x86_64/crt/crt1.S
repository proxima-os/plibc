.section ".text._start", "ax", @progbits

.balign 16
.globl _start
.hidden _start
.type _start, @function
_start:
    .cfi_startproc
    .cfi_undefined rip
    .cfi_undefined rsp

    xor %ebp, %ebp
    mov %rsp, %rdi
    mov main@GOTPCREL(%rip), %rsi
    # rdx is set by startup code
    call *__plibc_start@GOTPCREL(%rip)
    ud2

    .cfi_endproc
.size _start, . - _start

.section ".note.GNU-stack", "", @progbits
