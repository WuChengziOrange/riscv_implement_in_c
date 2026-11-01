#include "wbu.h"
#include "mem.h"
#include "my_riscv.h"

wbu_output wbu_reg = {0};

wbu_output wb_comb(cpu_state* pcpu, mem_output mem_out)
{
    wbu_reg.wb_have_inst = mem_out.valid;
    wbu_reg.wb_ena = mem_out.reg_write_en;
    wbu_reg.wb_pc = mem_out.pc;
    wbu_reg.wb_reg = mem_out.rd;
    wbu_reg.wb_value = mem_out.rd_val;

    if (wbu_reg.wb_ena && wbu_reg.wb_have_inst) {
        if (wbu_reg.wb_reg) {
            pcpu->gpr[wbu_reg.wb_reg] = wbu_reg.wb_value;

        } else {
            pcpu->gpr[0] = 0;
        }
    }
    return wbu_reg;
}