/*
 * syscalls.c —— newlib-nano 的底层重定向实现
 *
 * newlib 的 printf 最终会走到：
 *     printf -> _vfprintf_r -> __sfvwrite_r -> _write
 * 内存分配走到：
 *     malloc -> _malloc_r -> _sbrk_r -> _sbrk
 *
 * 所以裸机上最少必须实现 _write 和 _sbrk；
 * 其余是为了让链接器不报 undefined reference 而提供的桩函数。
 *
 * 说明：链接时我们同时加了 --specs=nosys.specs 兜底，
 *       libnosys 里是弱一点的“什么都不做”的实现，
 *       但本文件里的是强符号，会优先被采用（.o 先于 .a 被搜索）。
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>

/* ---------------- 硬件相关：按你的 UART 改这里 ---------------- */
#define UART0_BASE 0x80000000UL
#define UART0_TX   (*(volatile uint32_t *)(UART0_BASE + 0x0)) /* 写一个字节即发送 */
#define UART0_RX   (*(volatile uint32_t *)(UART0_BASE + 0x4)) /* 读一个字节即接收 */

void uart_putc(char c)
{
    UART0_TX = (uint32_t)(uint8_t)c;
}

void uart_puts(const char *s)
{
    while (*s)
        uart_putc(*s++);
}
/* ------------------------------------------------------------- */

/* 1) _write：printf / puts / fwrite 的最终落地函数 */
int _write(int fd, const char *buf, int len)
{
    (void)fd;
    for (int i = 0; i < len; i++) {
        if (buf[i] == '\n')          /* 串口终端习惯：\n -> \r\n */
            uart_putc('\r');
        uart_putc(buf[i]);
    }
    return len;
}

/* 2) _sbrk：堆分配，供 newlib stdio 缓冲区 / malloc 使用。
 *    约定符号 __heap_start / __heap_end 由 linker.ld 提供。 */
extern char __heap_start;
extern char __heap_end;

static char *heap_ptr = NULL;

void *_sbrk(ptrdiff_t incr)
{
    if (heap_ptr == NULL)
        heap_ptr = &__heap_start;

    char *prev = heap_ptr;
    char *next = prev + incr;

    if (next > &__heap_end) {
        errno = ENOMEM;
        return (void *)-1;
    }
    heap_ptr = next;
    return (void *)prev;
}

/* 3) _read：scanf / getchar 用。若你的 UART 没有 ready 标志位，
 *    这里假设读 UART0_RX 会阻塞到有数据为止。不需要可保留原样。 */
int _read(int fd, void *buf, size_t len)
{
    (void)fd;
    char *p = (char *)buf;
    size_t i = 0;
    while (i < len) {
        char c = (char)UART0_RX;
        p[i++] = c;
        if (c == '\n' || c == '\r')
            break;
    }
    return (int)i;
}

/* 4) 其余桩函数：告诉 newlib “这个文件描述符是终端，但无可 seek / 无文件系统” */
int _close(int fd)
{
    (void)fd;
    return -1;
}

int _fstat(int fd, struct stat *st)
{
    (void)fd;
    st->st_mode = S_IFCHR;   /* 字符设备 */
    return 0;
}

int _isatty(int fd)
{
    (void)fd;
    return 1;
}

off_t _lseek(int fd, off_t offset, int whence)
{
    (void)fd;
    (void)offset;
    (void)whence;
    return 0;
}

/* 5) 进程相关：裸机没有进程，做成空实现 */
void _exit(int code)
{
    (void)code;
    for (;;) {
        __asm__ volatile("nop");
    }
}

int _kill(int pid, int sig)
{
    (void)pid;
    (void)sig;
    errno = EINVAL;
    return -1;
}

int _getpid(void)
{
    return 1;
}

/* 少数新版本 newlib 的 arc4random / assert 路径会引用到，一并给个实现 */
int _getentropy(void *buf, size_t len)
{
    (void)buf;
    (void)len;
    errno = ENOSYS;
    return -1;
}
