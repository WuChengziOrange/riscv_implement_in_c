#include "emu.h"
#include "my_riscv.h"
#include <stdio.h>
#include <stdlib.h>

#define CYCLE_LIMIT 1000000
int main(int argc, char *argv[])
{
    const char* binpath = EMU_BIN_PATH;
    int i = 0;
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <program-file>\n", argv[0]);
        fprintf(stderr, "Example: %s riscv_test.bin\n", argv[0]);
    } else {
        binpath = argv[1];
    }
    init_cpu(binpath);
    init_my_riscv(binpath);
    WB_info golden = {0};
    wbu_output my_out = {0};
    printf("[difftest] Test Start!\n");

    for (i = 0; i < CYCLE_LIMIT; i++) {
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
            if((golden.wb_ena == 0) && (my_out.wb_ena == 0)) {
                continue;
            }
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
    printf("Timed out! Please check whether your CPU got stuck.%d >= %d\n",i, CYCLE_LIMIT);
    return 0;
}