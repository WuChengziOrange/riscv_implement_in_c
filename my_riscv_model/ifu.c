#include "stdint.h"

#include "my_riscv.h"

ifu_output ifu_reg = {0, 0, 0};

ifu_output IFU(cpu_state* pcpu)
{
    if (pcpu->ctrl.is_stall) {
        /* stall happend, just return last status,skip update combinational logic */
        return ifu_reg;
    }
    /* flip-flop emulator:时钟沿到来，将缓存的寄存器先传给下级 */
    ifu_output ret = ifu_reg;
    /* update pc here */
    pcpu->pc = pcpu->npc;

    /* 模拟组合逻辑，准备下一拍数据 */
    ifu_reg.instruction = mem_read(pcpu->pc, 4);
    ifu_reg.pc = pcpu->pc;
    ifu_reg.snpc = pcpu->pc + 4;
    ifu_reg.valid = pcpu->ctrl.is_flush ? 0 : 1;

    return ret;
}
