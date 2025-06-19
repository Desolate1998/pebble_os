#!/bin/bash
qemu-system-i386 -cdrom build/Pebble_OS.iso -hda build/fat.img -boot d -serial tcp::4242,server,nowait -netdev user,id=n0 -device ne2k_pci,netdev=n0
