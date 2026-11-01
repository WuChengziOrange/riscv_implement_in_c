#include "emu.h"
#include "my_riscv.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    init_cpu(EMU_BIN_PATH);
    init_my_riscv(EMU_BIN_PATH);
    WB_info golden = {0};
    wbu_output my_out = {0};
    printf("[difftest] Test Start!\n");
    for (int i = 0; i < 1000000; i++) {
        for (int j = 0; j < 100; j++) {
            golden = cpu_run_once();
            if (golden.wb_have_inst) {
                break;
            }
        }
        for (int j = 0; j < 100; j++) {
            my_out = my_riscv_one_cycle();
            if (my_out.wb_have_inst) {
                break;
            }
        }
        if ((golden.wb_pc == my_out.wb_pc) &&
            (golden.wb_reg == my_out.wb_reg) &&
            (golden.wb_value == my_out.wb_value) &&
            (golden.wb_ena == my_out.wb_ena)) {
            continue;
        } else {
            printf("[difftest] Test Failed at PC=0x%8.8x\n", golden.wb_pc);
            printf("[difftest] Golden: reg=%d, value=0x%8.8x, ena=%d\n",
                   golden.wb_reg,
                   golden.wb_value,
                   golden.wb_ena);
            printf("[difftest] My CPU: reg=%d, value=0x%8.8x, ena=%d\n",
                   my_out.wb_reg,
                   my_out.wb_value,
                   my_out.wb_ena);
            return -1;
        }
    }
    printf("Timed out! Please check whether your CPU got stuck.\n");
    return 0;
}