.global keyboard_handler
.extern keyboard_interrupt_received

keyboard_handler:
    pusha                       # save all general-purpose registers
    inb $0x60, %al              # read scancode from keyboard port
    movb %al, %bl               # move scancode into %bl (8-bit)
    pushl %ebx                  # push scancode (zero-extended) on stack
    call keyboard_interrupt_received
    add $4, %esp                # clean up stack

    movb $0x20, %al             # send EOI to PIC
    outb %al, $0x20

    popa                        # restore all registers
    iret                        # return from interrupt
