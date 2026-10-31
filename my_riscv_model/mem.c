#include "mem.h"
#include "debug.h"
#include "exu.h"
#include "idu.h"
#include "my_riscv.h"
typedef struct
{
    uint32_t pc;
    uint32_t rd_val;
    uint32_t rd;  // destination register index, or data to store
    uint32_t reg_write_en;
    uint32_t valid;  // indicate the instruction is valid or not
    uint32_t npc;    // next pc
} mem_output;

mem_output mem_reg = {0};

mem_output MEM(cpu_state* pcpu, exu_output exu_out)
{
    if (exu_out.valid == 0) {
        return mem_reg;
    }
    /* flip-flop emulator:时钟沿到来，将缓存的寄存器先传给下级 */
    mem_output ret = mem_reg;
    /* 模拟组合逻辑，准备下一拍数据 */
    mem_reg.pc = exu_out.pc;
    mem_reg.npc = exu_out.jump_taken ? exu_out.dnpc : exu_out.snpc;
    pcpu->npc = mem_reg.npc;  // update npc to next pc, for next instruction fetch
    mem_reg.reg_write_en = 0;  // default not write to register

    if (exu_out.alu_op == ADD_ADDRESS) {
        uint32_t addr = exu_out.rd_val;
        uint32_t store_data = exu_out.rd;
        if (exu_out.mem_type == MEM_LB) {
            mem_reg.rd_val = (int8_t)(mem_read(addr, 1));
            mem_reg.reg_write_en = 1;
        } else if (exu_out.mem_type == MEM_LH) {
            mem_reg.rd_val = ((int16_t)mem_read(addr, 2));
            mem_reg.reg_write_en = 1;
        } else if (exu_out.mem_type == MEM_LW) {
            mem_reg.rd_val = mem_read(addr, 4);
            mem_reg.reg_write_en = 1;
        } else if (exu_out.mem_type == MEM_LBU) {
            mem_reg.rd_val = mem_read(addr, 1);
            mem_reg.reg_write_en = 1;
        } else if (exu_out.mem_type == MEM_LHU) {
            mem_reg.rd_val = mem_read(addr, 2);
            mem_reg.reg_write_en = 1;
        } else if (exu_out.mem_type == MEM_SB) {
            mem_write(addr, store_data, 1);
        } else if (exu_out.mem_type == MEM_SH) {
            mem_write(addr, store_data, 2);
        } else if (exu_out.mem_type == MEM_SW) {
            mem_write(addr, store_data, 4);
        }
    }

    return ret;
}