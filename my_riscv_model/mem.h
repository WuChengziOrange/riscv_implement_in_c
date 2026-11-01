#ifndef MEM_H
#define MEM_H

#include "exu.h"

typedef struct
{
    uint32_t pc;
    uint32_t rd_val;
    uint32_t rd;  // destination register index, or data to store
    uint32_t reg_write_en;
    uint32_t valid;  // indicate the instruction is valid or not
    uint32_t is_load;
} mem_output;

mem_output mem_comb(cpu_state* pcpu, exu_output exu_out);

#endif  // !MEM_H