#include <stdint.h>
uint32_t data[10];
int main(void)
{
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
    return 0;
}