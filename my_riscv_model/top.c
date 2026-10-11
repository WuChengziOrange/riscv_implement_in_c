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

uint32_t mem_read(uint32_t addr)
{
    Assert(addr < MEM_SZ, "Memory read out of boundary!\n");
    return memory[addr >> 2];
}

void mem_write(uint32_t addr, uint32_t data)
{
    Assert(addr < MEM_SZ, "Memory write out of boundary!\n");
    memory[addr >> 2] = data;
}
