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

# Rust configuration
RUST_MANIFEST := rust/Cargo.toml
RUST_TARGET_JSON := rust/i686-pebble.json
RUST_TARGET_DIR := rust/target/i686-pebble
RUST_LIB := $(RUST_TARGET_DIR)/release/libcommand_driver.a

CFLAGS := -ffreestanding -O2 -Wall -Wextra -std=gnu99 -Ikernel -Ikernel/lib/std
CXXFLAGS := -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -std=gnu++17 -Ikernel -Ikernel/lib/std
LDFLAGS := -T $(LINKER_SCRIPT) -ffreestanding -O2 -nostdlib

# Hardcoded libgcc path
LIBGCC := /usr/local/cross/lib/libgcc.a

# === Files ===
C_SRC := $(shell find $(KERNEL_DIR) -name '*.c')
CPP_SRC := $(shell find $(KERNEL_DIR) -name '*.cpp')
ASM_SRC := $(shell find $(KERNEL_DIR) $(BOOT_DIR) -name '*.s')

OBJ := $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SRC)) \
       $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(CPP_SRC)) \
       $(patsubst %.s, $(BUILD_DIR)/%.o, $(ASM_SRC))

# Include Rust staticlib in objects so link rule picks it up as dependency
OBJ += $(RUST_LIB)

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
	mkdir -p $(BUILD_DIR)/kernel/lib/std

# Compile C
$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C++
$(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Assemble
$(BUILD_DIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(AS) $< -o $@

# Link kernel binary
$(BUILD_DIR)/$(OUTPUT_BIN): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBGCC)

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

# Build Rust crate for freestanding target
$(RUST_LIB): $(RUST_MANIFEST) $(RUST_TARGET_JSON)
	@echo "Building Rust command driver (nightly, build-std)..."
	cd rust && cargo +nightly build -Z build-std=core,compiler_builtins --manifest-path Cargo.toml --target i686-pebble.json --release

clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR)

.PHONY: all clean dirs
