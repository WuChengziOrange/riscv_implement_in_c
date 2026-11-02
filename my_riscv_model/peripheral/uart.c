#include "uart.h"
#include "stdio.h"

uint32_t uart_read(peripheral_t* self, uint32_t addr, uint8_t bytes)
{
    uint32_t* pmem = self->pool;
    // Adjust address to be relative to the peripheral's base address
    addr -= self->base_addr;
    return bus_read(pmem, addr, bytes);
}

void uart_write(peripheral_t* self, uint32_t addr, uint32_t data, uint8_t bytes)
{
    uint32_t* pmem = self->pool;
    // Adjust address to be relative to the peripheral's base address
    addr -= self->base_addr;
    bus_write(pmem, addr, data, bytes);
    if (addr == UART_TX_OFFSET) {
        // Handle UART transmission (for example, print to console)
        // Output the least significant byte to the console
        putchar(data & 0xFF);
    }
}