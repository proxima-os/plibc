.text

#ifndef NDEBUG
.cfi_sections .debug_frame
#endif

.balign 16
.globl rtld_start
.type rtld_start, @function
rtld_start:
    .cfi_startproc
    .cfi_undefined rip
    .cfi_undefined rsp
    mov %rsp, %rdi
    lea _DYNAMIC(%rip), %rsi
    call rtld_relocate_self
    xor %edx, %edx
    jmp _start
    .cfi_endproc
.size rtld_start, . - rtld_start

.balign 16
.globl rtld_self_lazy_thunk
.type rtld_self_lazy_thunk, @function
rtld_self_lazy_thunk:
    # The initial call instruction pushes one 8-byte value, and the PLT code pushes two more, so right now the stack
    # is aligned to 8 bytes. The least significant nibble of the offset here must be 8 to realign the stack to 16.
    sub $0x48, %rsp
    mov %rax, 0x00(%rsp)
    mov %rcx, 0x08(%rsp)
    mov %rdx, 0x10(%rsp)
    mov %rsi, 0x18(%rsp)
    mov %rdi, 0x20(%rsp)
    mov %r8, 0x28(%rsp)
    mov %r9, 0x30(%rsp)
    mov %r10, 0x38(%rsp)
    mov %r11, 0x40(%rsp)

    mov 0x50(%rsp), %rdi # relocation index
    call rtld_self_lazy
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
    add $0x50, %rsp # earlier offset + 8 to get rid of one of the values pushed by the plt code
    ret # the remaining value that was pushed by the plt code was replaced with the target address
.size rtld_self_lazy_thunk, . - rtld_self_lazy_thunk
