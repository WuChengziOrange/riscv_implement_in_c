#include "idu.h"
#include "debug.h"
#include "my_riscv.h"

#define SIGN_EXTEND(x, bits) ((int32_t)(x << (32 - bits)) >> (32 - bits))
idu_output idu_reg = {0};

void idu_type_r(cpu_state* pcpu, uint32_t inst)
{
    R_type r_inst = *(R_type*)&inst;
    idu_reg.rd = r_inst.rd;
    idu_reg.rs1 = r_inst.rs1;
    idu_reg.rs2 = r_inst.rs2;
    idu_reg.alu_input1 = gpr_read(pcpu, idu_reg.rs1);
    idu_reg.alu_input2 = gpr_read(pcpu, idu_reg.rs2);
    uint32_t sub_opcode = (r_inst.funct7 << 3) | r_inst.funct3;
    switch (sub_opcode) {
        case 0b0000000000:  // ADD
            idu_reg.alu_op = ADD;
            break;
        case 0b0100000000:  // SUB
            idu_reg.alu_op = SUB;
            break;
        case 0b0000000001:  // SLL
            idu_reg.alu_op = SLL;
            break;
        case 0b0000000010:  // SLT
            idu_reg.alu_op = SLT;
            break;
        case 0b0000000011:  // SLTU
            idu_reg.alu_op = SLTU;
            break;
        case 0b0000000100:  // XOR
            idu_reg.alu_op = XOR;
            break;
        case 0b0000000101:  // SRL
            idu_reg.alu_op = SRL;
            break;
        case 0b0100000101:  // SRA
            idu_reg.alu_op = SRA;
            break;
        case 0b0000000110:  // OR
            idu_reg.alu_op = OR;
            break;
        case 0b0000000111:  // AND
            idu_reg.alu_op = AND;
            break;
        default:
            idu_reg.alu_op = INVALID;
    }
}

void idu_type_i(cpu_state* pcpu, uint32_t inst)
{
    I_type i_inst = *(I_type*)&inst;

    idu_reg.rd = i_inst.rd;
    idu_reg.rs1 = i_inst.rs1;
    idu_reg.alu_input1 = gpr_read(pcpu, idu_reg.rs1);
    idu_reg.alu_input2 = SIGN_EXTEND(i_inst.imm, 12);
    switch (i_inst.funct3) {
        case 0b000:  // ADDI
            idu_reg.alu_op = ADD;
            break;
        case 0b010:  // SLTI
            idu_reg.alu_op = SLT;
            break;
        case 0b011:  // SLTIU
            idu_reg.alu_op = SLTU;
            break;
        case 0b100:  // XORI
            idu_reg.alu_op = XOR;
            break;
        case 0b110:  // ORI
            idu_reg.alu_op = OR;
            break;
        case 0b111:  // ANDI
            idu_reg.alu_op = AND;
            break;
        default:
            idu_reg.alu_op = INVALID;
    }
}

void idu_type_i_mem(cpu_state* pcpu, uint32_t inst)
{
    I_type i_inst = *(I_type*)&inst;
    idu_reg.rd = i_inst.rd;
    idu_reg.rs1 = i_inst.rs1;
    idu_reg.alu_input1 = gpr_read(pcpu, idu_reg.rs1);
    idu_reg.alu_input2 = SIGN_EXTEND(i_inst.imm, 12);
    idu_reg.alu_op = ADD_ADDRESS;
    idu_reg.is_load = 1;

    switch (i_inst.funct3) {
        case 0b000:  // LB
            idu_reg.mem_type = MEM_LB;
            break;
        case 0b001:  // LH
            idu_reg.mem_type = MEM_LH;
            break;
        case 0b010:  // LW
            idu_reg.mem_type = MEM_LW;
            break;
        case 0b100:  // LBU
            idu_reg.mem_type = MEM_LBU;
            break;
        case 0b101:  // LHU
            idu_reg.mem_type = MEM_LHU;
            break;
        default:
            idu_reg.alu_op = INVALID;
    }
}

void idu_type_i_jalr(cpu_state* pcpu, uint32_t inst)
{
    I_type i_inst = *(I_type*)&inst;

    idu_reg.rd = i_inst.rd;
    idu_reg.rs1 = i_inst.rs1;
    idu_reg.alu_input1 = gpr_read(pcpu, idu_reg.rs1);
    idu_reg.alu_input2 = SIGN_EXTEND(i_inst.imm, 12);
    if (i_inst.funct3 == 0b000) {  // JALR
        idu_reg.alu_op = ADD_JUMP;
    } else {
        idu_reg.alu_op = INVALID;
    }
}

void idu_type_i_fence(cpu_state* pcpu, uint32_t inst)
{
    idu_reg.reg_write_en = 0;
    idu_reg.rd = 0;

    panic("no support for FENCE instruction");
}

void idu_type_i_ecall(cpu_state* pcpu, uint32_t inst)
{
    idu_reg.reg_write_en = 0;
    idu_reg.rd = 0;
    idu_reg.alu_op = ECALL;
    // TODO: ebreak, csr, and other system instructions
}

void idu_type_s(cpu_state* pcpu, uint32_t inst)
{
    S_type s_inst = *(S_type*)&inst;
    idu_reg.reg_write_en = 0;  // store instruction don't write back
    idu_reg.rd = 0;
    idu_reg.rs1 = s_inst.rs1;
    idu_reg.rs2 = s_inst.rs2;
    idu_reg.is_store = 1;

    idu_reg.mem_data = gpr_read(pcpu, idu_reg.rs2);
    idu_reg.alu_input1 = gpr_read(pcpu, idu_reg.rs1);
    idu_reg.alu_input2 = (s_inst.imm11_5 << 5) | s_inst.imm4_0;
    idu_reg.alu_input2 = SIGN_EXTEND(idu_reg.alu_input2, 12);

    idu_reg.alu_op = ADD_ADDRESS;

    switch (s_inst.funct3) {
        case 0b000:  // SB
            idu_reg.mem_type = MEM_SB;
            break;
        case 0b001:  // SH
            idu_reg.mem_type = MEM_SH;
            break;
        case 0b010:  // SW
            idu_reg.mem_type = MEM_SW;
            break;
        default:
            idu_reg.alu_op = INVALID;
    }
}

void idu_type_b(cpu_state* pcpu, uint32_t inst, uint32_t pc)
{
    B_type b_inst = *(B_type*)&inst;
    idu_reg.reg_write_en = 0;
    idu_reg.rd = 0;
    idu_reg.rs1 = b_inst.rs1;
    idu_reg.rs2 = b_inst.rs2;

    idu_reg.alu_input1 = gpr_read(pcpu, idu_reg.rs1);
    idu_reg.alu_input2 = gpr_read(pcpu, idu_reg.rs2);
    idu_reg.dnpc = (((b_inst.imm12 << 12) | (b_inst.imm11 << 11) |
                     (b_inst.imm10_5 << 5) | (b_inst.imm4_1 << 1)) +
                    pc) &
                   0xfffffffe;  // branch target address
    idu_reg.alu_input2 = SIGN_EXTEND(idu_reg.alu_input2, 12);

    switch (b_inst.funct3) {
        case 0b000:  // BEQ
            idu_reg.alu_op = BEQ;
            break;
        case 0b001:  // BNE
            idu_reg.alu_op = BNE;
            break;
        case 0b100:  // BLT
            idu_reg.alu_op = BLT;
            break;
        case 0b101:  // BGE
            idu_reg.alu_op = BGE;
            break;
        case 0b110:  // BLTU
            idu_reg.alu_op = BLTU;
            break;
        case 0b111:  // BGEU
            idu_reg.alu_op = BGEU;
            break;
        default:
            idu_reg.alu_op = INVALID;
    }
}

void idu_type_u(cpu_state* pcpu, uint32_t inst, uint32_t pc)
{
    U_type u_inst = *(U_type*)&inst;

    idu_reg.rd = u_inst.rd;
    idu_reg.alu_input1 = 0;  // for LUI, AUIPC, the first input is 0 or PC
    idu_reg.alu_input2 = (u_inst.imm31_12 << 12);
    idu_reg.alu_op = ADD;
    if (u_inst.opcode == 0x17) {  // AUIPC
        idu_reg.alu_input1 = pc;
    } else {
        idu_reg.alu_op = INVALID;
    }
}

void idu_type_j(cpu_state* pcpu, uint32_t inst, uint32_t pc)
{
    J_type j_inst = *(J_type*)&inst;

    idu_reg.rd = j_inst.rd;
    idu_reg.alu_input1 = pc;
    idu_reg.alu_input2 = (j_inst.imm20 << 20) | (j_inst.imm19_12 << 12) |
                         (j_inst.imm11 << 11) | (j_inst.imm10_1 << 1);
    idu_reg.alu_input2 = SIGN_EXTEND(idu_reg.alu_input2, 21);

    idu_reg.alu_op = ADD_JUMP;
}

void inst_type_check(cpu_state* pcpu, uint32_t inst, uint32_t pc)
{
    uint32_t opcode = inst & 0x7F;
    switch (opcode) {
        case 0x33:  // R-type
            idu_type_r(pcpu, inst);
            break;
        case 0x13:  // I-type
            idu_type_i(pcpu, inst);
            break;
        case 0x3:  // Load I-type
            idu_type_i_mem(pcpu, inst);
            break;
        case 0x23:  // S-type
            idu_type_s(pcpu, inst);
            break;
        case 0x63:  // B-type
            idu_type_b(pcpu, inst, pc);
            break;
        case 0x37:  // LUI U-type
            idu_type_u(pcpu, inst, pc);
            break;
        case 0x17:  // AUIPC U-type
            idu_type_u(pcpu, inst, pc);
            break;
        case 0x6F:  // JAL J-type
            idu_type_j(pcpu, inst, pc);
            break;
        case 0x67:  // JALR I-type
            idu_type_i_jalr(pcpu, inst);
            break;
        case 0x0f:  // FENCE I-type
            idu_type_i_fence(pcpu, inst);
            break;
        case 0x73:  // ECALL I-type
            idu_type_i_ecall(pcpu, inst);
            break;
        default:  // TODO: unhandled instruction type
            panic("Invalid Insturction %8.8x at PC=%8.8x\n", inst, pc);
    }
}

idu_output id_comb(cpu_state* pcpu, ifu_output ifu)
{
    /* 模拟组合逻辑，准备下一拍数据 */
    idu_reg.reg_write_en = 1;  // default 1, most instructions will write back
    idu_reg.rd = idu_reg.rs1 = idu_reg.rs2 = 0;  // default 0
    idu_reg.is_load = 0;   // default it's not load instruction
    idu_reg.is_store = 0;  // default it's not store instruction
    idu_reg.pc = ifu.pc;
    idu_reg.snpc = ifu.snpc;
    idu_reg.valid = ifu.valid;
    if(idu_reg.valid == 0) {
        return idu_reg;
    }
    inst_type_check(pcpu, ifu.instruction, ifu.pc);

    return idu_reg;
}