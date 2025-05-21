.section ".text.__plibc_start", "ax", @progbits

.weak _DYNAMIC
.weak _GLOBAL_OFFSET_TABLE_

.balign 16
.globl __plibc_start
.type __plibc_start, @function
__plibc_start:
    .cfi_startproc

    sub $0x18, %rsp # this realigns the stack to 16 bytes for the call to __plibc_rtld_init
    .cfi_adjust_cfa_offset 0x18
    mov %rdi, 0x00(%rsp)
    mov %rsi, 0x08(%rsp)
    mov %rdx, 0x10(%rsp)
    .cfi_rel_offset rdi, 0x00
    .cfi_rel_offset rsi, 0x08
    .cfi_rel_offset rdx, 0x10

    lea _DYNAMIC(%rip), %rsi
    lea _GLOBAL_OFFSET_TABLE_(%rip), %rdx
    call __plibc_rtld_init

    mov 0x00(%rsp), %rdi
    mov 0x08(%rsp), %rsi
    mov 0x10(%rsp), %rdx
    add $0x18, %rsp
    .cfi_adjust_cfa_offset -0x18
    .cfi_restore rdi
    .cfi_restore rsi
    .cfi_restore rdx
    jmp __plibc_main
    .cfi_endproc
.size __plibc_start, . - __plibc_start

.section ".note.GNU-stack", "", @progbits
