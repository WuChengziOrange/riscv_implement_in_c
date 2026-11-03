CROSS_COMPILE ?= riscv-none-elf-

CC      := $(CROSS_COMPILE)gcc
OBJCOPY := $(CROSS_COMPILE)objcopy
OBJDUMP := $(CROSS_COMPILE)objdump
SIZE    := $(CROSS_COMPILE)size

BUILD_DIR := build
TARGET    := $(BUILD_DIR)/firmware
BIN       := $(BUILD_DIR)/firmware.bin
MAP       := $(TARGET).map

# -mcmodel=medany：让 la/lui 能寻址 0x80000000 这类高地址（UART 就在那儿）
ARCH    := -march=rv32i -mabi=ilp32 -mcmodel=medany

OPT     ?= -O0 -g

# -ffreestanding  ：没有宿主环境，不要假设有 libc 运行时
# -fno-builtin    ：不要把我们自己的 _write/printf 之类替换成内建展开
# -fno-pie        ：裸机不走 PIC，避免 GOT/PLT
CFLAGS  := $(ARCH) $(OPT) \
           -ffreestanding -fno-builtin \
           -fno-pie \
           -ffunction-sections -fdata-sections \
           -Wall -Wextra \
           -MMD -MP

# ---- 关键：链接 newlib-nano ----
# --specs=nano.specs : 换成 libc_nano.a / libg_nano.a（省 flash，printf 不支持 %f）
# --specs=nosys.specs: 自动加 -lnosys，提供所有 syscall 的空实现兜底，
#                      避免 "undefined reference to _close/_lseek/..." 之类报错
# -nostartfiles      : 不要 crt0，入口用我们自己的 _start（但不影响 -lc 的链接）
LDFLAGS := $(ARCH) \
           -nostartfiles \
           -fno-pie -no-pie \
           --specs=nano.specs \
           --specs=nosys.specs \
           -T linker.ld \
           -Wl,-e,_start \
           -Wl,--gc-sections \
           -Wl,--no-warn-rwx-segments \
           -Wl,--print-memory-usage \
           -Wl,-Map,$(MAP)

LDLIBS  :=

# 需要 printf("%f") / scanf("%f") 时取消注释（各多占约 5~8KB）
# LDFLAGS += -Wl,-u,_printf_float
# LDFLAGS += -Wl,-u,_scanf_float

SRCS := $(wildcard *.c)
ASMS := $(wildcard *.S)
OBJS := $(addprefix $(BUILD_DIR)/,$(SRCS:.c=.o) $(ASMS:.S=.o))
DEPS := $(OBJS:.o=.d)

.PHONY: all clean disasm size

all: $(BIN) size

$(BUILD_DIR):
	@mkdir -p $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.S | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS) linker.ld | $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

$(BIN): $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

size: $(TARGET).elf
	@$(SIZE) $<

disasm: $(TARGET).elf
	$(OBJDUMP) -d -S $<

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
