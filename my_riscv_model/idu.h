#ifndef IDU_H
#define IDU_H

#include "ifu.h"
#include "stdint.h"

typedef struct
{
    uint32_t opcode : 7;
    uint32_t rd : 5;
    uint32_t funct3 : 3;
    uint32_t rs1 : 5;
    uint32_t rs2 : 5;
    uint32_t funct7 : 7;
} R_type;

typedef struct
{
    uint32_t opcode : 7;
    uint32_t rd : 5;
    uint32_t funct3 : 3;
    uint32_t rs1 : 5;
    uint32_t imm : 12;
} I_type;

typedef struct
{
    uint32_t opcode : 7;
    uint32_t imm4_0 : 5;
    uint32_t funct3 : 3;
    uint32_t rs1 : 5;
    uint32_t rs2 : 5;
    uint32_t imm11_5 : 7;
} S_type;

typedef struct
{
    uint32_t opcode : 7;
    uint32_t imm11 : 1;
    uint32_t imm4_1 : 4;
    uint32_t funct3 : 3;
    uint32_t rs1 : 5;
    uint32_t rs2 : 5;
    uint32_t imm10_5 : 6;
    uint32_t imm12 : 1;
} B_type;

typedef struct
{
    uint32_t opcode : 7;
    uint32_t rd : 5;
    uint32_t imm31_12 : 20;
} U_type;

typedef struct
{
    uint32_t opcode : 7;
    uint32_t rd : 5;
    uint32_t imm19_12 : 8;
    uint32_t imm11 : 1;
    uint32_t imm10_1 : 10;
    uint32_t imm20 : 1;
} J_type;

enum
{
    ADD = 0,
    SUB,
    SLL,
    SLT,
    SLTU,
    XOR,
    SRL,
    SRA,
    OR,
    AND,
    ADD_ADDRESS,  // for load/store address calculation
    ADD_JUMP,     // for jump address calculation
    ECALL,
    BEQ,
    BNE,
    BLT,
    BGE,
    BLTU,
    BGEU,
    INVALID
};
enum
{
    MEM_LB = 0,
    MEM_LH,
    MEM_LW,
    MEM_LBU,
    MEM_LHU,
    MEM_SB,
    MEM_SH,
    MEM_SW
};

typedef struct
{
    uint32_t pc;
    uint32_t valid;
    // 1.indicate the destination register number, 2. store data to memory
    uint32_t rd;  // destination register index
    uint32_t rs1;
    uint32_t rs2;
    uint32_t mem_data;      // data to store to memory
    uint32_t reg_write_en;  // indicate whether write back to register file
    uint32_t alu_input1;
    uint32_t alu_input2;
    uint32_t alu_op;
    uint32_t is_load;
    uint32_t is_store;
    uint32_t mem_type;
    uint32_t snpc;
    uint32_t dnpc;
} idu_output;

idu_output id_comb(cpu_state* pcpu, ifu_output ifu);

#endif
