OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY
{
    /* 如果将来改成从 Flash 运行，把 .text/.rodata 放到 ROM 区，并给 .data 加 AT() 即可 */
    RAM (rwx) : ORIGIN = 0x00000000, LENGTH = 64K
}

/* 栈大小，位于 RAM 最高端，向下生长 */
STACK_SIZE = 4K;

_stack_top  = ORIGIN(RAM) + LENGTH(RAM);
_stack_bottom = _stack_top - STACK_SIZE;

SECTIONS
{
    . = ORIGIN(RAM);

    .text : ALIGN(4)
    {
        KEEP(*(.text.start))
        *(.text .text.*)
    } > RAM

    .rodata : ALIGN(4)
    {
        *(.rodata .rodata.*)
        *(.srodata .srodata.*)
    } > RAM

    /* 单段 RAM、映像整体加载到 0x0，因此 .data 的 VMA == LMA，无需搬运。
       若改成 Flash + RAM 分离，见文件末尾说明。 */
    .data : ALIGN(4)
    {
        __data_start = .;
        *(.data .data.*)
        *(.sdata .sdata.*)
        . = ALIGN(4);
        __data_end = .;
    } > RAM

    /* C++ 全局构造 / __attribute__((constructor)) 用；纯 C 也建议保留，
       因为 __libc_init_array() 需要这四个符号存在 */
    .preinit_array : ALIGN(4)
    {
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN(__preinit_array_end = .);
    } > RAM

    .init_array : ALIGN(4)
    {
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN(__init_array_end = .);
    } > RAM

    .fini_array : ALIGN(4)
    {
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
    } > RAM

    /* .bss 放在镜像末尾（NOLOAD），这样 bin 文件不会把 bss 的空间也填成 0 */
    .bss (NOLOAD) : ALIGN(4)
    {
        __bss_start = .;
        *(.bss .bss.*)
        *(.sbss .sbss.*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end = .;
    } > RAM

    /* ---- 堆：给 newlib 的 malloc 用，_sbrk() 在这里分配 ---- */
    . = ALIGN(16);
    __heap_start = .;
    __heap_end   = _stack_bottom;
    PROVIDE(end  = __heap_start);   /* newlib 约定符号 */
    PROVIDE(_end = __heap_start);

    ASSERT(__heap_start < __heap_end, "no room for heap: reduce STACK_SIZE or RAM usage")

    /* 丢弃不需要的段，避免 objcopy 出的 bin 膨胀 */
    /DISCARD/ :
    {
        *(.comment)
        *(.note .note.*)
        *(.eh_frame .eh_frame_hdr)
    }
}

/*
 * 若将来改为「Flash(XIP) + RAM」双区布局，把 .data 改成：
 *
 *   .data : ALIGN(4)
 *   {
 *       __data_start = .;
 *       *(.data .data.*)
 *       . = ALIGN(4);
 *       __data_end = .;
 *   } > RAM AT> FLASH
 *   __data_load_start = LOADADDR(.data);
 *
 * 并在 startup.S 里把 __data_load_start -> __data_start 的 __data_end-__data_start
 * 字节拷贝过去（在清 bss 之前做）。
 */
