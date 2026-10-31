#ifndef EXU_H
#define EXU_H

#include "stdint.h"

typedef struct
{
    uint32_t rd_val;
    uint32_t rd;    // destination register index, or data to store
    uint32_t alu_op;
    uint32_t valid;
    uint32_t pc;
    uint32_t snpc;
    uint32_t dnpc;
    uint32_t mem_type;
    uint32_t jump_taken;

} exu_output;

#endif