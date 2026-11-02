CROSS_COMPILE ?= riscv64-unknown-elf-

CC      := $(CROSS_COMPILE)gcc
OBJCOPY := $(CROSS_COMPILE)objcopy
OBJDUMP := $(CROSS_COMPILE)objdump

BUILD_DIR := build
TARGET    := $(BUILD_DIR)/firmware
BIN       := $(BUILD_DIR)/firmware.bin

CFLAGS := -march=rv32i -mabi=ilp32 \
          -O0 -g \
          -ffreestanding -fno-builtin \
          -fno-pie -no-pie \
          -ffunction-sections -fdata-sections \
          -Wall -Wextra

LDFLAGS := -march=rv32i -mabi=ilp32 \
           -nostdlib -nostartfiles \
           -T linker.ld \
           -Wl,-e,_start \
           -Wl,--gc-sections \
           -Wl,-Map,$(TARGET).map

OBJECTS := $(BUILD_DIR)/startup.o $(BUILD_DIR)/main.o

.PHONY: all clean disasm

all: $(BIN)

$(BUILD_DIR):
    mkdir -p $@

$(BUILD_DIR)/startup.o: startup.S | $(BUILD_DIR)
    $(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/main.o: main.c | $(BUILD_DIR)
    $(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJECTS) linker.ld
    $(CC) $(LDFLAGS) -o $@ $(OBJECTS) -lgcc

$(BIN): $(TARGET).elf
    $(OBJCOPY) -O binary $< $@

disasm: $(TARGET).elf
    $(OBJDUMP) -d -S $<

clean:
    rm -rf $(BUILD_DIR)