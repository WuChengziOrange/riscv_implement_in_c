#include "sram.h"

uint32_t ram_read(peripheral_t* self, uint32_t addr, uint8_t bytes)
{
    uint32_t* pmem = self->pool;
    // Adjust address to be relative to the peripheral's base address
    addr -= self->base_addr;
    return bus_read(pmem, addr, bytes);
}

void ram_write(peripheral_t* self, uint32_t addr, uint32_t data, uint8_t bytes)
{
    uint32_t* pmem = self->pool;
    // Adjust address to be relative to the peripheral's base address
    addr -= self->base_addr;
    bus_write(pmem, addr, data, bytes);
}