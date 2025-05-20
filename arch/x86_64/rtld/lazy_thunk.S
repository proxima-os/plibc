#ifndef LAZY_PREFIX
#define LAZY_PREFIX
#endif

#define CONCAT3(a, b, c) a ##b ##c
#define NAME2(middle, suffix) CONCAT3(__plibc_rtld_, middle, suffix)
#define NAME(suffix) NAME2(LAZY_PREFIX, lazy##suffix)

.text

.balign 16
.globl NAME(_thunk)
.hidden NAME(_thunk)
.type NAME(_thunk), @function
NAME(_thunk):
    .cfi_startproc
    .cfi_def_cfa rsp, 0x18
    .cfi_offset rip, -0x08

    # realign stack to 16 bytes for call
    sub $0x48, %rsp
    .cfi_adjust_cfa_offset 0x48
    mov %rax, 0x00(%rsp)
    mov %rcx, 0x08(%rsp)
    mov %rdx, 0x10(%rsp)
    mov %rsi, 0x18(%rsp)
    mov %rdi, 0x20(%rsp)
    mov %r8, 0x28(%rsp)
    mov %r9, 0x30(%rsp)
    mov %r10, 0x38(%rsp)
    mov %r11, 0x40(%rsp)
    .cfi_rel_offset rax, 0x00
    .cfi_rel_offset rcx, 0x08
    .cfi_rel_offset rdx, 0x10
    .cfi_rel_offset rsi, 0x18
    .cfi_rel_offset rdi, 0x20
    .cfi_rel_offset r8, 0x28
    .cfi_rel_offset r9, 0x30
    .cfi_rel_offset r10, 0x38
    .cfi_rel_offset r11, 0x40

    mov 0x48(%rsp), %rdi
    mov 0x50(%rsp), %rsi
    call NAME()
    mov %rax, 0x50(%rsp) # prepare return address

    mov 0x00(%rsp), %rax
    mov 0x08(%rsp), %rcx
    mov 0x10(%rsp), %rdx
    mov 0x18(%rsp), %rsi
    mov 0x20(%rsp), %rdi
    mov 0x28(%rsp), %r8
    mov 0x30(%rsp), %r9
    mov 0x38(%rsp), %r10
    mov 0x40(%rsp), %r11
    add $0x50, %rsp
    .cfi_adjust_cfa_offset -0x50
    .cfi_restore rax
    .cfi_restore rcx
    .cfi_restore rdx
    .cfi_restore rsi
    .cfi_restore rdi
    .cfi_restore r8
    .cfi_restore r9
    .cfi_restore r10
    .cfi_restore r11
    ret
    .cfi_endproc
.size NAME(_thunk), . - NAME(_thunk)

.section ".note.GNU-stack", "", @progbits
