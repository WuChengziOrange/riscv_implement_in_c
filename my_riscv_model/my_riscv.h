#ifndef MY_RISCV_H
#define MY_RISCV_H
#include "stdint.h"

typedef struct
{
    uint32_t gpr[32];
    uint32_t pc;
    uint32_t npc;
} riscv32_CPU_state;

uint32_t mem_read(uint32_t addr);
void mem_write(uint32_t addr, uint32_t data);

#endif