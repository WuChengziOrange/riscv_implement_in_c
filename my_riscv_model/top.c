#include "stdint.h"
#include "debug.h"

#define MEM_SZ 64000
static uint32_t memory[MEM_SZ / sizeof(uint32_t)];

void mem_init(const char *fname)
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

uint32_t mem_read(uint32_t addr,uint8_t bytes)
{
    Assert(addr < MEM_SZ, "Memory read out of boundary!\n");
    uint32_t index = addr >> 2;
    uint32_t byte_off = addr & 0b11;
    uint32_t bit_off = byte_off * 8;
    uint32_t result = memory[index];
    switch(bytes)
    {
        case 1:
            result = (result >> bit_off) & 0xFF;
            return result;
        case 2:
            result = (result >> bit_off) & 0xFFFF;
            if(byte_off >= 3) {
                assert(0, "Memory read unaligned!\n");
            }
            return result;
        case 4:
            if(byte_off != 0) {
                assert(0, "Memory read unaligned!\n");
            }
            return result;
        default:
            assert(0, "Memory read bytes must be 1,2,4!\n");
    }
}

void mem_write(uint32_t addr, uint32_t data, uint8_t bytes)
{
    Assert(addr < MEM_SZ, "Memory write out of boundary!\n");
    uint32_t index = addr >> 2;
    uint32_t byte_off = addr & 0b11;
    uint32_t bit_off = byte_off * 8;
    uint32_t oldVal = memory[index];
    switch(bytes)
    {
        case 1:
            memory[index] = (oldVal & (~(0xFF << bit_off))) | ((data & 0xFF) << bit_off);
            return;
        case 2:
            if(byte_off >= 3) {
                assert(0, "Memory write unaligned!\n");
            }
            memory[index] = (oldVal & (~(0xFFFF << bit_off))) | ((data & 0xFFFF) << bit_off);
            return;
        case 4:
            if(byte_off != 0) {
                assert(0, "Memory write unaligned!\n");
            }
            memory[index] = data;
            return;
        default:
            assert(0, "Memory write bytes must be 1,2,4!\n");
    }
}
