#include <stdint.h>
typedef struct {
    volatile uint32_t tx;
    volatile uint32_t rx;
} uart_t;

uart_t* uart0 = (uart_t*)0x80000000;

uint32_t data[10];
int main(void)
{
    char str[] = "\r\ngood evening\r\n";
    uint32_t a = 0x12345678;
    uint32_t b = 0x87654321;
    uint32_t c = a + b;
    uint32_t d = a - b;
    uint32_t e = a & b;
    uint32_t f = a | b;
    data[0] = c;
    data[1] = d;
    data[2] = e;
    data[3] = f;
    for(uint32_t i = 0; i < sizeof(str); i++) {
        uart0->tx = (uint32_t)str[i];
    }
    return 0;
}