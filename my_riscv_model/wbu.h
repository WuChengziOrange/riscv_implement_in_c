#ifndef WBU_H
#define WBU_H

#include "mem.h"
#include "stdint.h"

typedef struct
{
    uint32_t wb_have_inst;
    uint32_t wb_pc;
    uint32_t wb_reg;
    uint32_t wb_value;
    uint32_t wb_ena;
} wbu_output;

wbu_output wb_comb(cpu_state* pcpu, mem_output mem_out);

#endif  // !WBU_H