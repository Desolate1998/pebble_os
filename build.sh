#!/bin/bash
set -e

OS_NAME="Pebble OS"
BUILD_DIR="build"
ISO_DIR="iso"
GRUB_DIR="$ISO_DIR/boot/grub"
OUTPUT_BIN="Pebble.bin"
LINKER_SCRIPT="linker.ld"
BOOT_DIR="boot"
KERNEL_DIR="kernel"

echo "🔧 Building $OS_NAME..."

mkdir -p "$BUILD_DIR"
mkdir -p "$ISO_DIR/boot/grub"

# Compile all .cpp and .c files in kernel directory
OBJ_FILES=()
for SRC in "$KERNEL_DIR"/*.{c,cpp}; do
  [[ -e "$SRC" ]] || continue
  OBJ="$BUILD_DIR/$(basename "${SRC%.*}").o"
  if [[ "$SRC" == *.c ]]; then
    i686-elf-gcc -c "$SRC" -o "$OBJ" -ffreestanding -O2 -Wall -Wextra -std=gnu99
  else
    i686-elf-g++ -c "$SRC" -o "$OBJ" -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -std=gnu++17
  fi
  OBJ_FILES+=("$OBJ")
done

# Assemble all .s files in boot directory
for SRC in "$BOOT_DIR"/*.s; do
  [[ -e "$SRC" ]] || continue
  OBJ="$BUILD_DIR/$(basename "${SRC%.*}").o"
  i686-elf-as "$SRC" -o "$OBJ"
  OBJ_FILES+=("$OBJ")
done

# Link everything
i686-elf-g++ -T "$LINKER_SCRIPT" -o "$BUILD_DIR/$OUTPUT_BIN" -ffreestanding -O2 -nostdlib "${OBJ_FILES[@]}" -lgcc

# Copy kernel binary to ISO directory
cp "$BUILD_DIR/$OUTPUT_BIN" "$ISO_DIR/boot/$OUTPUT_BIN"

# Create GRUB config
cat > "$GRUB_DIR/grub.cfg" << EOF
menuentry "$OS_NAME" {
    multiboot /boot/$OUTPUT_BIN
    boot
}
EOF

# Build the ISO
grub-mkrescue -o "${OS_NAME// /_}.iso" "$ISO_DIR"

echo "✅ Build complete: ${OS_NAME// /_}.iso is ready to boot"
