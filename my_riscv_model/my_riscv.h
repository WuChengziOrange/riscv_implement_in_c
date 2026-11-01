#ifndef MY_RISCV_H
#define MY_RISCV_H

#include "stdint.h"

typedef struct
{
    uint8_t is_jmp_taken;
    uint8_t is_stall;
    uint8_t is_flush;
} control_state;

typedef struct
{
    uint32_t gpr[32];
    uint32_t pc;
    // uint32_t dnpc;
    uint32_t snpc;
    uint32_t csr[4];
    control_state ctrl;
    void* regs;
} cpu_state;

enum
{
    R_TYPE = 0,
    I_TYPE,
    S_TYPE,
    B_TYPE,
    U_TYPE,
    J_TYPE,
    INVALID_TYPE
};
typedef struct
{
    uint32_t wb_have_inst;
    uint32_t wb_pc;
    uint32_t wb_reg;
    uint32_t wb_value;
    uint32_t wb_ena;
} wbu_output;

uint32_t mem_read(uint32_t addr, uint8_t bytes);
void mem_write(uint32_t addr, uint32_t data, uint8_t bytes);
uint32_t gpr_read(cpu_state* pcpu, uint32_t r);
void init_my_riscv(const char* fname);
wbu_output my_riscv_one_cycle();

#endif