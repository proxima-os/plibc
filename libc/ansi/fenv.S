.section ".text.fegetround", "ax", @progbits

.balign 16
.globl fegetround
.protected fegetround
.type fegetround, @function
fegetround:
    stmxcsr -4(%rsp)
    mov -4(%rsp), %eax
    and $0x6000, %eax
    ret
.size fegetround, . - fegetround

.section ".text.fesetround", "ax", @progbits

.balign 16
.globl fesetround
.protected fesetround
.type fesetround, @function
fesetround:
    # set sse rounding mode
    stmxcsr -4(%rsp)
    mov -4(%rsp), %eax
    andl $~0x6000, -4(%rsp)
    orl %edi, -4(%rsp)
    ldmxcsr -4(%rsp)

    # set x87 rounding mode
    shr $3, %edi
    fstcw -2(%rsp)
    andw $~0xc00, -2(%rsp)
    orw %di, -2(%rsp)
    fldcw -2(%rsp)
    ret
.size fesetround, . - fesetround
