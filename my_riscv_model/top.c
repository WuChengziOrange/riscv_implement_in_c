#include "exu.h"
#include "idu.h"
#include "ifu.h"
#include "mem.h"
#include "my_riscv.h"
#include "peripheral.h"
#include "stdint.h"
#include "wbu.h"

static const char* bin_name;

typedef struct
{
    ifu_output if_id;
    idu_output id_ex;
    exu_output ex_mem;
    mem_output mem_wb;
} pipeline_reg;

pipeline_reg regs = {0};

static cpu_state cpu = {.regs = &regs};

uint32_t gpr_read(cpu_state* pcpu, uint32_t r)
{
    pipeline_reg* reg = pcpu->regs;
    if (reg->mem_wb.reg_write_en) {
        /* in this case wb state is writing register file */
        if ((reg->mem_wb.rd == r) && r) {
            return reg->mem_wb.rd_val;
        }
    }
    /* most common case */
    return pcpu->gpr[r];
}

wbu_output my_riscv_one_cycle()
{
    /* 1. combine logic */
    ifu_output if_out = if_comb(&cpu);
    idu_output id_out = id_comb(&cpu, regs.if_id);
    exu_output ex_out = ex_comb(&cpu, regs.id_ex);
    mem_output mem_out = mem_comb(&cpu, regs.ex_mem);
    wbu_output wb_out = wb_comb(&cpu, regs.mem_wb);
    cpu.ctrl.is_stall = 0;

    /*  if | id | ex | mem| wb */
    /*       if | id | ex | mem| wb */
    /*            if | id | ex | mem| wb */
    /*                 if | id | ex | mem| wb */
    /*                      if | id | ex | mem| wb */

    /* 2. control unit */
    /* forwarding type 1: wb_input to ex_input */
    if (mem_out.reg_write_en && mem_out.rd && mem_out.valid) {
        if (mem_out.rd == id_out.rs1) {
            id_out.alu_input1 = mem_out.rd_val;
        }
        if (mem_out.rd == id_out.rs2) {
            if (id_out.is_store) {
                /* store instruction, the data to store is in mem_data */
                id_out.mem_data = mem_out.rd_val;
            } else {
                id_out.alu_input2 = mem_out.rd_val;
            }
        }
    }
    if (ex_out.rd && ex_out.valid) {
        if (ex_out.is_load) {
            /* stall check */
            cpu.ctrl.is_stall =
                (ex_out.rd == id_out.rs1) || (ex_out.rd == id_out.rs2);
        } else {
            /* forwarding type 2: mem_input to ex_input */
            if (ex_out.rd == id_out.rs1) {
                id_out.alu_input1 = ex_out.rd_val;
            }
            if (ex_out.rd == id_out.rs2) {
                if (id_out.is_store) {
                    /* store instruction, the data to store is in mem_data */
                    id_out.mem_data = ex_out.rd_val;
                } else {
                    id_out.alu_input2 = ex_out.rd_val;
                }
            }
        }
    }

    if (cpu.ctrl.is_stall) {
        /* stall signal will generate in idu, so if_id not change, id_ex fill nop */
        regs.id_ex.valid = 0;  // let valid to zero
    } else {
        // most normal case
        regs.if_id = if_out;
        regs.id_ex = id_out;
        /* 4. pc update */
        cpu.pc = cpu.snpc;
        if (ex_out.jump_taken) {
            /* fill nop */
            regs.if_id.valid = 0;
            regs.id_ex.valid = 0;
            cpu.pc = ex_out.dnpc;
        }
    }
    regs.ex_mem = ex_out;
    regs.mem_wb = mem_out;

    return wb_out;
}

void init_my_riscv(const char* fname)
{
    cpu.snpc = 0;
    bin_name = fname;
    load_bin(fname);
}

const char* my_get_bin_name(void)
{
    return bin_name;
}