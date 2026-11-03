    .section .text.start, "ax", @progbits
    .globl  _start
    .type   _start, @function

_start:
    /* 1) 栈指针：newlib 的 printf 内部要用到栈，栈必须最先设置 */
    la      sp, _stack_top

    /* 2) 清零 .bss（newlib 的 stdio 依赖全局/静态变量初值为 0） */
    la      t0, __bss_start
    la      t1, __bss_end
1:
    bgeu    t0, t1, 2f
    sw      zero, 0(t0)
    addi    t0, t0, 4
    j       1b
2:

    /* 3) 运行 C++ 全局构造 / __attribute__((constructor))。
          纯 C 工程也建议调用，顺带把 newlib 的内部初始化跑一遍 */
    call    __libc_init_array

    /* 4) 进 main */
    call    main
    ecall # ecall as end of program
    /* 5) main 返回：交给 newlib 的 exit()。
          它会先把 printf 的缓冲区 flush 掉，再调用我们实现的 _exit()，
          所以即使去掉 main 里的 setvbuf 也不会丢输出。 */
    call    exit

    /* 兜底：万一 _exit 被换掉，停在这里而不是乱跑 */
3:
    j       3b

    .size   _start, . - _start
