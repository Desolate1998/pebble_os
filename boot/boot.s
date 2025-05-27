.section .multiboot
.align 4
.long 0x1BADB002                  # Multiboot magic number
.long 0                         # Flags (no special features)
.long -(0x1BADB002 + 0)         # Checksum

.section .text
.global _start
.extern kernel_main

_start:
    call kernel_main              # Call kernel entry point

hang:
    cli                          # Disable interrupts
    hlt                          # Halt CPU
    jmp hang                     # Loop forever

