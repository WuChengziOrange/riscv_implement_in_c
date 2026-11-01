#include "mem.h"
#include "debug.h"
#include "exu.h"
#include "idu.h"
#include "my_riscv.h"


mem_output mem_reg = {0};

mem_output mem_comb(cpu_state* pcpu, exu_output exu_out)
{
    /* 模拟组合逻辑，准备下一拍数据 */
    mem_reg.pc = exu_out.pc;
    mem_reg.reg_write_en = exu_out.reg_write_en;
    mem_reg.valid = exu_out.valid;
    mem_reg.rd = exu_out.rd;
    mem_reg.is_load = exu_out.is_load;
    if (!mem_reg.valid) {
        return mem_reg;
    }
    if (exu_out.alu_op == ADD_ADDRESS) {
        uint32_t addr = exu_out.rd_val;
        uint32_t store_data = exu_out.mem_data;
        if (exu_out.mem_type == MEM_LB) {
            mem_reg.rd_val = (int8_t)(mem_read(addr, 1));
        } else if (exu_out.mem_type == MEM_LH) {
            mem_reg.rd_val = ((int16_t)mem_read(addr, 2));
        } else if (exu_out.mem_type == MEM_LW) {
            mem_reg.rd_val = mem_read(addr, 4);
        } else if (exu_out.mem_type == MEM_LBU) {
            mem_reg.rd_val = mem_read(addr, 1);
        } else if (exu_out.mem_type == MEM_LHU) {
            mem_reg.rd_val = mem_read(addr, 2);
        } else if (exu_out.mem_type == MEM_SB) {
            mem_write(addr, store_data, 1);
        } else if (exu_out.mem_type == MEM_SH) {
            mem_write(addr, store_data, 2);
        } else if (exu_out.mem_type == MEM_SW) {
            mem_write(addr, store_data, 4);
        }
    }

    return mem_reg;
}