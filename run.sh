#!/bin/bash
set -e

# Convert line endings just in case
sed -i 's/\r$//' run.sh

# Clean C++ build
make clean

# Build Rust library for kernel
cargo build \
  --manifest-path rust/Cargo.toml \
  --release \
  --target rust/i686-pebble.json \
  -Z build-std=core,compiler_builtins \
  -Z build-std-features=compiler-builtins-mem

# Build C++ kernel and ISO
make

# Boot QEMU
qemu-system-i386 -cdrom ./build/Pebble_OS.iso
