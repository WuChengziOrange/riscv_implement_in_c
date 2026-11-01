#include "exu.h"
#include "debug.h"
#include "my_riscv.h"
#include "stdint.h"
#include <stdlib.h>

exu_output exu_reg = {0};
const char* my_get_bin_name(void);

int _ecall_checker(cpu_state* pcpu)
{  // check the testpoint
    printf("\r\nbin: %s\r\n",my_get_bin_name());

    if (pcpu->gpr[10] == 0) {
        printf("my Test Point Pass!\n");
        return 0;
    } else {
        printf("my Test Point Failed\n");
        return -1;
    }
}
exu_output ex_comb(cpu_state* pcpu, idu_output idu_out)
{

    /* 模拟组合逻辑，准备下一拍数据 */
    exu_reg.rd = idu_out.rd;
    exu_reg.jump_taken = 0;  // default not jump taken
    exu_reg.dnpc = idu_out.dnpc; // default for branch instruction
    exu_reg.pc = idu_out.pc;
    exu_reg.snpc = idu_out.snpc;
    exu_reg.valid = idu_out.valid;
    exu_reg.alu_op = idu_out.alu_op;
    exu_reg.reg_write_en = idu_out.reg_write_en;
    exu_reg.mem_data = idu_out.mem_data;
    exu_reg.is_load = idu_out.is_load;
    if (!exu_reg.valid) {
        /* not valid quick return */
        return exu_reg;
    }
    switch (idu_out.alu_op) {
        case ADD:
            exu_reg.rd_val = idu_out.alu_input1 + idu_out.alu_input2;
            break;
        case SUB:
            exu_reg.rd_val = idu_out.alu_input1 - idu_out.alu_input2;
            break;
        case SLL:
            exu_reg.rd_val = idu_out.alu_input1 << (idu_out.alu_input2 & 0x1f);
            break;
        case SLT:
            exu_reg.rd_val =
                (int32_t)idu_out.alu_input1 < (int32_t)idu_out.alu_input2 ? 1
                                                                          : 0;
            break;
        case SLTU:
            exu_reg.rd_val = idu_out.alu_input1 < idu_out.alu_input2 ? 1 : 0;
            break;
        case XOR:
            exu_reg.rd_val = idu_out.alu_input1 ^ idu_out.alu_input2;
            break;
        case SRL:
            exu_reg.rd_val = idu_out.alu_input1 >> (idu_out.alu_input2 & 0x1f);
            break;
        case SRA:
            exu_reg.rd_val =
                (int32_t)idu_out.alu_input1 >> (idu_out.alu_input2 & 0x1f);
            break;
        case OR:
            exu_reg.rd_val = idu_out.alu_input1 | idu_out.alu_input2;
            break;
        case AND:
            exu_reg.rd_val = idu_out.alu_input1 & idu_out.alu_input2;
            break;
        case ADD_ADDRESS:
            exu_reg.rd_val = idu_out.alu_input1 +
                             idu_out.alu_input2;  // mem address calculation
            exu_reg.mem_type = idu_out.mem_type;
            break;
        case ADD_JUMP:
            exu_reg.dnpc = (idu_out.alu_input1 + idu_out.alu_input2) &
                           0xfffffffe;  // jump address calculation
            exu_reg.rd_val = idu_out.snpc;
            exu_reg.jump_taken = 1;
            break;
        case ECALL:  // TODO: ECALL handling
            color_print("ECALL at PC = 0x%8.8x, Stop now.\n", idu_out.pc);
            exit(_ecall_checker(pcpu));
            break;
        case BEQ:
            exu_reg.jump_taken = (idu_out.alu_input1 == idu_out.alu_input2);
            break;
        case BNE:
            exu_reg.jump_taken = (idu_out.alu_input1 != idu_out.alu_input2);
            break;
        case BLT:
            exu_reg.jump_taken = ((int32_t)idu_out.alu_input1 < (int32_t)idu_out.alu_input2);
            break;
        case BGE:
            exu_reg.jump_taken = ((int32_t)idu_out.alu_input1 >= (int32_t)idu_out.alu_input2);
            break;
        case BLTU:
            exu_reg.jump_taken = (idu_out.alu_input1 < idu_out.alu_input2);
            break;
        case BGEU:
            exu_reg.jump_taken = (idu_out.alu_input1 >= idu_out.alu_input2);
            break;
        default:
            panic("Invalid ALU operation %8.8x at PC=%8.8x\n",
                  idu_out.alu_op,
                  idu_out.pc);
    }

    return exu_reg;
}
