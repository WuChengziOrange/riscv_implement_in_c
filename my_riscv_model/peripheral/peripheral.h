#ifndef PERIPHERAL_H
#define PERIPHERAL_H
#include <stdint.h>

typedef struct peripheral_t
{
    uint32_t* pool;
    uint32_t base_addr;
    uint32_t len;
    uint32_t (*callback_r)(struct peripheral_t*, uint32_t addr, uint8_t bytes);
    void (*callback_w)(struct peripheral_t*,
                       uint32_t addr,
                       uint32_t data,
                       uint8_t bytes);
} peripheral_t;

uint32_t bus_read(uint32_t* pmem, uint32_t addr, uint8_t bytes);
void bus_write(uint32_t* pmem, uint32_t addr, uint32_t data, uint8_t bytes);

uint32_t mem_read(uint32_t addr, uint8_t bytes);
void mem_write(uint32_t addr, uint32_t data, uint8_t bytes);
void load_bin(const char* fname);

#endif  // !PERIPHERAL_H
