.section .text.start
.globl _start
_start:
    # 初始化栈指针
    la sp, _stack_top
    la t0, __bss_start
    la t1, __bss_end

_bss_clear:
    bgeu t0, t1, _call_main
    sw zero, 0(t0)
    addi t0, t0, 4
    j _bss_clear
_call_main:
    call main
    ecall # ecall as end of program

_forever_loop:
    j _forever_loop