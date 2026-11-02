#include "ifu.h"
#include "peripheral.h"
#include "stdint.h"
ifu_output ifu_reg = {0};

ifu_output if_comb(cpu_state* pcpu)
{
    /* 模拟组合逻辑，准备下一拍数据 */
    ifu_reg.instruction = mem_read(pcpu->pc, 4);
    pcpu->snpc = pcpu->pc + 4;
    ifu_reg.pc = pcpu->pc;
    ifu_reg.snpc = pcpu->snpc;
    ifu_reg.valid = 1;
    ifu_reg.valid = ifu_reg.instruction != 0;// this is for test
    //color_print("\r\nMyRisc IF: 0x%8.8x at PC=0x%8.8x", ifu_reg.instruction, ifu_reg.pc);
    return ifu_reg;
}
