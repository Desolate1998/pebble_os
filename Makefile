# === Config ===
OS_NAME := Pebble_OS
BUILD_DIR := build
ISO_DIR := iso
GRUB_DIR := $(ISO_DIR)/boot/grub
BOOT_DIR := boot
KERNEL_DIR := kernel
OUTPUT_BIN := $(OS_NAME).bin
LINKER_SCRIPT := linker.ld

# === Tools ===
AS := i686-elf-as
CC := i686-elf-gcc
CXX := i686-elf-g++
LD := i686-elf-g++
GRUB_MKRESCUE := grub-mkrescue

CFLAGS := -ffreestanding -O2 -Wall -Wextra -std=gnu99 -Ikernel
CXXFLAGS := -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -std=gnu++17 -Ikernel
LDFLAGS := -T $(LINKER_SCRIPT) -ffreestanding -O2 -nostdlib -lgcc

# === Files ===
C_SRC := $(shell find $(KERNEL_DIR) -name '*.c')
CPP_SRC := $(shell find $(KERNEL_DIR) -name '*.cpp')
ASM_SRC := $(shell find $(KERNEL_DIR) $(BOOT_DIR) -name '*.s')

OBJ := $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SRC)) \
       $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(CPP_SRC)) \
       $(patsubst %.s, $(BUILD_DIR)/%.o, $(ASM_SRC))

ISO := $(BUILD_DIR)/$(OS_NAME).iso

# === Rules ===
all: dirs $(ISO)

dirs:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(ISO_DIR)/boot/grub
	mkdir -p $(ISO_DIR)/boot
	mkdir -p $(BUILD_DIR)/boot
	mkdir -p $(BUILD_DIR)/kernel/core/logger
	mkdir -p $(BUILD_DIR)/kernel/core
	mkdir -p $(BUILD_DIR)/kernel/drivers/keyboard
	mkdir -p $(BUILD_DIR)/kernel/drivers/terminal
	mkdir -p $(BUILD_DIR)/kernel/interrupts/idt

# Compile C
$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C++
$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Assemble
$(BUILD_DIR)/%.o: %.s
	$(AS) $< -o $@

# Link kernel binary
$(BUILD_DIR)/$(OUTPUT_BIN): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

# GRUB config
$(GRUB_DIR)/grub.cfg:
	echo 'set timeout=5' > $@
	echo 'set default=0' >> $@
	echo 'menuentry "$(OS_NAME)" {' >> $@
	echo '    multiboot /boot/$(OUTPUT_BIN)' >> $@
	echo '    boot' >> $@
	echo '}' >> $@

# Copy kernel binary into ISO tree
$(ISO_DIR)/boot/$(OUTPUT_BIN): $(BUILD_DIR)/$(OUTPUT_BIN) | $(ISO_DIR)/boot
	cp $< $@

# Build ISO
$(ISO): $(ISO_DIR)/boot/$(OUTPUT_BIN) $(GRUB_DIR)/grub.cfg
	$(GRUB_MKRESCUE) -o $@ $(ISO_DIR)

clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR)

.PHONY: all clean dirs