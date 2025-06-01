.section .multiboot
.align 4
.long 0x1BADB002                # Multiboot magic number required by bootloader
.long 0                         # Multiboot flags (0 = no special features)
.long -(0x1BADB002 + 0)         # Checksum

.section .bss
    .lcomm stack, 16384         # Reserve 16 KB stack

.section .text
.global _start
.extern kernel_main
.extern gdt_install             # Your GDT install function

_start:
    call gdt_install           # Setup GDT (must reload segment registers)
    mov $stack + 16384, %esp   # Setup stack pointer
    call kernel_main           # Call kernel main

hang:
    cli
    hlt
    jmp hang
