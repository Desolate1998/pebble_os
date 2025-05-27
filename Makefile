# === Configuration ===
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

CFLAGS := -ffreestanding -O2 -Wall -Wextra -std=gnu99
CXXFLAGS := -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -std=gnu++17
LDFLAGS := -T $(LINKER_SCRIPT) -ffreestanding -O2 -nostdlib -lgcc

# === File Lists ===
C_SRC := $(wildcard $(KERNEL_DIR)/*.c)
CPP_SRC := $(wildcard $(KERNEL_DIR)/*.cpp)
ASM_SRC := $(wildcard $(BOOT_DIR)/*.s)

OBJ := $(patsubst $(KERNEL_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SRC)) \
       $(patsubst $(KERNEL_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CPP_SRC)) \
       $(patsubst $(BOOT_DIR)/%.s, $(BUILD_DIR)/%.o, $(ASM_SRC))

ISO := $(OS_NAME).iso

# === Targets ===

all: $(ISO)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(ISO_DIR)/boot $(GRUB_DIR):
	mkdir -p $(GRUB_DIR)

# Compile C source
$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C++ source
$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Assemble ASM source
$(BUILD_DIR)/%.o: $(BOOT_DIR)/%.s | $(BUILD_DIR)
	$(AS) $< -o $@

# Link all objects
$(BUILD_DIR)/$(OUTPUT_BIN): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

# Create GRUB config
$(GRUB_DIR)/grub.cfg: | $(GRUB_DIR)
	echo 'menuentry "$(OS_NAME)" {' > $@
	echo '    multiboot /boot/$(OUTPUT_BIN)' >> $@
	echo '    boot' }>> $@

# Copy binary to ISO dir
$(ISO_DIR)/boot/$(OUTPUT_BIN): $(BUILD_DIR)/$(OUTPUT_BIN) | $(ISO_DIR)/boot
	cp $< $@

# Build ISO
$(ISO): $(ISO_DIR)/boot/$(OUTPUT_BIN) $(GRUB_DIR)/grub.cfg
	$(GRUB_MKRESCUE) -o $@ $(ISO_DIR)

clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR) $(ISO)

.PHONY: all clean
