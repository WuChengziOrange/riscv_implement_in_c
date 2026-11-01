#include "ifu.h"
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

    return ifu_reg;
}
