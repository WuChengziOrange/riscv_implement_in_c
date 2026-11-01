#ifndef IFU_H
#define IFU_H

#include "my_riscv.h"
#include "stdint.h"

typedef struct
{
    uint32_t instruction;
    uint32_t pc;
    uint32_t snpc;
    uint32_t valid;
} ifu_output;

ifu_output if_comb(cpu_state* pcpu);

#endif