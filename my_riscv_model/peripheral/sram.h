#ifndef SRAM_H
#define SRAM_H

#include "peripheral.h"

uint32_t ram_read(peripheral_t* self, uint32_t addr, uint8_t bytes);
void ram_write(peripheral_t* self, uint32_t addr, uint32_t data, uint8_t bytes);
#endif
