#include "exu.h"
#include "debug.h"
#include "idu.h"
#include "my_riscv.h"
#include "stdint.h"
#include <stdlib.h>

exu_output exu_reg = {0, 0};

int _ecall_checker(cpu_state* pcpu)
{  // check the testpoint
    if (pcpu->gpr[10] == 0) {
        printf("Test Point Pass!\n");
        return 0;
    } else {
        printf("Test Point Failed\n");
        return -1;
    }
}
exu_output EXU(cpu_state* pcpu, idu_output idu_out)
{
    if (pcpu->ctrl.is_stall) {
        /* stall happend, just return last status,skip update combinational logic */
        return exu_reg;
    }
    /* flip-flop emulator:时钟沿到来，将缓存的寄存器先传给下级 */
    exu_output ret = exu_reg;

    /* 模拟组合逻辑，准备下一拍数据 */
    exu_reg.rd = idu_out.alu_res;
    exu_reg.jump_taken = 0;  // default not jump taken
    exu_reg.dnpc = idu_out.dnpc; // default for branch instruction
    exu_reg.pc = idu_out.pc;
    exu_reg.snpc = idu_out.snpc;
    exu_reg.valid = idu_out.valid;
    exu_reg.alu_op = idu_out.alu_op;

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
    pcpu->npc = exu_reg.jump_taken ? exu_reg.dnpc : exu_reg.snpc;  // update npc to next pc, for next instruction fetch

    return ret;
}
