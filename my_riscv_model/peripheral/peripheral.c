#include "peripheral.h"
#include "debug.h"
#include "sram.h"
#include "uart.h"

#define SRAM_SZ 65536
static uint32_t sram[SRAM_SZ / sizeof(uint32_t)];
static uint32_t uart_pool[2];       // UART peripheral has 2 registers
static uint32_t seven_seg_pool[1];  // 7-segment display has only one register

uint32_t bus_read(uint32_t* pmem, uint32_t addr, uint8_t bytes)
{
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

void bus_write(uint32_t* pmem, uint32_t addr, uint32_t data, uint8_t bytes)
{
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

uint8_t address_in_peripheral(uint32_t addr, peripheral_t* peripheral)
{
    return ((addr >= peripheral->base_addr) &&
            (addr < (peripheral->base_addr + peripheral->len)));
}

peripheral_t peripheral_list[] = {
    {
        .pool = sram,
        .base_addr = 0,
        .len = SRAM_SZ,
        .callback_r = ram_read,
        .callback_w = ram_write,
    },
    {
        .pool = uart_pool,
        .base_addr = 0x80000000,
        .len = 8,
        .callback_r = uart_read,
        .callback_w = uart_write,
    },
    {
        .pool = seven_seg_pool,
        .base_addr = 0xfffff000,
        .len = 4,
        .callback_r = ram_read, /* now just treat as ram */
        .callback_w = ram_write,
    },
};

void load_bin(const char* fname)
{
    assert(fname != NULL);
    FILE* fp = fopen(fname, "rb");
    Assert(fp != NULL, "Cannot open file \"%s\"!\n", fname);

    Log_color("Using file \"%s\" as memory image.\n", fname);
    fseek(fp, 0, SEEK_END);
    int img_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    int ret = fread(sram, img_size, 1, fp);
    assert(ret == 1);
    fclose(fp);
}

uint32_t mem_read(uint32_t addr, uint8_t bytes)
{
    int peripheral_count = sizeof(peripheral_list) / sizeof(peripheral_t);
    for (int i = 0; i < peripheral_count; i++) {
        peripheral_t* peripheral = &peripheral_list[i];
        if (address_in_peripheral(addr, peripheral)) {
            return peripheral->callback_r(peripheral, addr, bytes);
        }
    }
    panic("Invalid memory read address! %8.8x\n", addr);
}

void mem_write(uint32_t addr, uint32_t data, uint8_t bytes)
{
    int peripheral_count = sizeof(peripheral_list) / sizeof(peripheral_t);
    for (int i = 0; i < peripheral_count; i++) {
        peripheral_t* peripheral = &peripheral_list[i];
        if (address_in_peripheral(addr, peripheral)) {
            peripheral->callback_w(peripheral, addr, data, bytes);
            return;
        }
    }
    panic("Invalid memory write address! %8.8x\n", addr);
}