#include <stdio.h>
#include <stdlib.h>
#include "emu.h"

int main(void) {

    init_cpu(EMU_BIN_PATH);

    printf("[difftest] Test Start!\n");
    for(int i = 0; i < 1000000; i++) {
        cpu_run_once();
    }
    printf("Timed out! Please check whether your CPU got stuck.\n");
    return 0;
}