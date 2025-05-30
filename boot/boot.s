.section .multiboot
.align 4
.long 0x1BADB002                # Multiboot magic number required by bootloader
.long 0                         # Multiboot flags (0 = no special features)
.long -(0x1BADB002 + 0)         # Checksum (must make sum of magic, flags, checksum = 0)

.section .text
.global _start                   # Entry point visible to linker/loader
.extern kernel_main              # Kernel main function implemented in C/C++

_start:
    call kernel_main             # Call the kernel's main entry function

hang:
    cli                         # Clear interrupts flag - disable interrupts
    hlt                         # Halt CPU until next interrupt (low power)
    jmp hang                    # Infinite loop to keep CPU halted indefinitely
