#ifndef UART_H
#define UART_H
#include "peripheral.h"

enum
{
    UART_TX_OFFSET = 0,
    UART_RX_OFFSET = 4,
};

uint32_t uart_read(peripheral_t* self, uint32_t addr, uint8_t bytes);
void uart_write(peripheral_t* self,
                uint32_t addr,
                uint32_t data,
                uint8_t bytes);
#endif  // !UART_H