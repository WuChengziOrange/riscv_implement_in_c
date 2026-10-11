#ifndef __EMU_H__
#define __EMU_H__

#include <stdint.h>
#include "include/cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

void init_cpu(const char *fname);
void print_reg_state(void);
void register_peripheral(const char *name,
                         uint32_t base_addr,
                         uint32_t len,
                         PeripheralRCallback callback_r,
                         PeripheralWCallback callback_w);
WB_info cpu_run_once(void);

#ifdef __cplusplus
}
#endif

#endif
