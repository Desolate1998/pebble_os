# === Config ===
OS_NAME := Pebble_OS
BUILD_DIR := build
ISO_DIR := iso
GRUB_DIR := $(ISO_DIR)/boot/grub
BOOT_DIR := boot
KERNEL_DIR := kernel
OUTPUT_BIN := Pebble.bin
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
ASM_SRC := $(wildcard $(BOOT_DIR)/*.s)

OBJ := $(patsubst $(KERNEL_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SRC)) \
       $(patsubst $(KERNEL_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CPP_SRC)) \
       $(patsubst $(BOOT_DIR)/%.s, $(BUILD_DIR)/%.o, $(ASM_SRC))

ISO := $(BUILD_DIR)/$(OS_NAME).iso

# === Rules ===

all: dirs $(ISO)

dirs:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(ISO_DIR)/boot/grub
	@$(foreach dir,$(sort $(dir $(OBJ))),mkdir -p $(dir);)

$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(BOOT_DIR)/%.s
	$(AS) $< -o $@

$(BUILD_DIR)/$(OUTPUT_BIN): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

$(GRUB_DIR)/grub.cfg:
	echo 'menuentry "$(OS_NAME)" {' > $@
	echo '    multiboot /boot/$(OUTPUT_BIN)' >> $@
	echo '    boot' >> $@
	echo '    }' >> $@

$(ISO_DIR)/boot/$(OUTPUT_BIN): $(BUILD_DIR)/$(OUTPUT_BIN) | $(ISO_DIR)/boot
	cp $< $@

$(ISO): $(ISO_DIR)/boot/$(OUTPUT_BIN) $(GRUB_DIR)/grub.cfg
	$(GRUB_MKRESCUE) -o $@ $(ISO_DIR)

clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR)

.PHONY: all clean dirs
