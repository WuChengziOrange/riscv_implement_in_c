#ifndef EXU_H
#define EXU_H

#include "stdint.h"
#include "idu.h"

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
    uint32_t reg_write_en;
    uint32_t mem_data;
    uint32_t is_load;
} exu_output;

exu_output ex_comb(cpu_state* pcpu, idu_output idu_out);

#endif