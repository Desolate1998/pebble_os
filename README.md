# Pebble OS

Pebble OS is a lightweight, experimental operating system designed from the ground up to function as a dedicated database platform. It combines a minimal kernel with custom components to efficiently manage and serve database operations directly on bare metal.

## Features

- Custom kernel with VGA text output
- Bootable ISO using GRUB
- Designed to prioritize database workloads
- Modular architecture for easy extension

## Building

Requirements:
- `i686-elf` cross-compiler toolchain
- `grub-mkrescue`

To build the OS and generate the bootable ISO, run:

```bash
make
