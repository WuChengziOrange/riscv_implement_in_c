#include "debug.h"
#include "my_riscv.h"

#include "exu.h"
#include "idu.h"
#include "ifu.h"
#include "mem.h"
#include "stdint.h"
#include "wbu.h"

static const char* bin_name;
#define MEM_SZ 65536
static uint32_t memory
    [MEM_SZ / sizeof(uint32_t) +
     3];  // 3 words for peripheral memory, 0x80000000~0x80000008, 0xFFFFF000~0xFFFFF004
typedef struct
{
    ifu_output if_id;
    idu_output id_ex;
    exu_output ex_mem;
    mem_output mem_wb;
} pipeline_reg;

pipeline_reg regs = {0};

static cpu_state cpu = {.regs = &regs};

void mem_init(const char* fname)
{
    assert(fname != NULL);
    FILE* fp = fopen(fname, "rb");
    Assert(fp != NULL, "Cannot open file \"%s\"!\n", fname);
    Log("Using file \"%s\" as memory image.\n", fname);
    fseek(fp, 0, SEEK_END);
    int img_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    int ret = fread(memory, img_size, 1, fp);
    assert(ret == 1);
    fclose(fp);
}
uint32_t* mem_remap(uint32_t* addr)
{
    uint32_t* pmem = memory;
    uint32_t maddr = *addr;
    if (maddr >= 0x80000000 && maddr < 0x80000008) {
        *addr = maddr - 0x80000000;
        pmem = memory + (MEM_SZ / sizeof(uint32_t));
    } else if (maddr >= 0xFFFFF000 && maddr < 0xFFFFF004) {
        *addr = maddr - 0xFFFFF000;
        pmem = memory + (MEM_SZ / sizeof(uint32_t)) + 2;
    } else if (maddr < MEM_SZ) {
        pmem = memory;
    } else {
        panic("Memory addr out of boundary! %8.8x >= %8.8x\n", maddr, MEM_SZ);
    }
    return pmem;
}
uint32_t mem_read(uint32_t addr, uint8_t bytes)
{
    uint32_t* pmem = mem_remap(&addr);
    uint32_t index = addr >> 2;
    uint32_t byte_off = addr & 0b11;
    uint32_t bit_off = byte_off * 8;
    uint32_t result = pmem[index];
    switch (bytes) {
        case 1:
            result = (result >> bit_off) & 0xFF;
            return result;
        case 2:
            result = (result >> bit_off) & 0xFFFF;
            if (byte_off >= 3) {
                panic("Memory read unaligned!\n");
            }
            return result;
        case 4:
            if (byte_off != 0) {
                panic("Memory read unaligned!\n");
            }
            return result;
        default:
            panic("Memory read bytes must be 1,2,4!\n");
    }
}

void mem_write(uint32_t addr, uint32_t data, uint8_t bytes)
{
    uint32_t* pmem = mem_remap(&addr);

    uint32_t index = addr >> 2;
    uint32_t byte_off = addr & 0b11;
    uint32_t bit_off = byte_off * 8;
    uint32_t oldVal = pmem[index];
    switch (bytes) {
        case 1:
            pmem[index] =
                (oldVal & (~(0xFF << bit_off))) | ((data & 0xFF) << bit_off);
            return;
        case 2:
            if (byte_off >= 3) {
                panic("Memory write unaligned!\n");
            }
            pmem[index] = (oldVal & (~(0xFFFF << bit_off))) |
                          ((data & 0xFFFF) << bit_off);
            return;
        case 4:
            if (byte_off != 0) {
                panic("Memory write unaligned!\n");
            }
            pmem[index] = data;
            return;
        default:
            panic("Memory write bytes must be 1,2,4!\n");
    }
}

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
    mem_init(fname);
}
const char* my_get_bin_name(void)
{
    return bin_name;
}