#include <stdio.h>
#include <stdint.h>

uint32_t data[10];

int main(void)
{
    /* 不需要缓冲：64K RAM 下省掉 1KB 的 stdio buffer，也省掉一次 malloc */
    setvbuf(stdout, NULL, _IONBF, 0);

    uint32_t a = 0x12345678;
    uint32_t b = 0x87654321;

    printf("\r\n=== hello newlib-nano ===\r\n");
    printf("a = 0x%08x (%u)\r\n", (unsigned)a, (unsigned)a);
    printf("b = 0x%08x (%u)\r\n", (unsigned)b, (unsigned)b);
    printf("a+b = 0x%08x\r\n", (unsigned)(a + b));
    printf("a-b = 0x%08x\r\n", (unsigned)(a - b));
    printf("a&b = 0x%08x\r\n", (unsigned)(a & b));
    printf("a|b = 0x%08x\r\n", (unsigned)(a | b));
    printf("pointer of data = %p\r\n", (void *)data);

    data[0] = a + b;
    printf("data[0] = %u\r\n", (unsigned)data[0]);

    return 0;
}
