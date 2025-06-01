.global keyboard_handler
.type keyboard_handler, @function

keyboard_handler:
    pusha                   # save all general-purpose registers

    inb $0x60, %al          # read scancode from keyboard data port (0x60)
    # You can move %al to somewhere if needed or call C handler

    movb $0x20, %al
    outb %al, $0x20         # send End Of Interrupt (EOI) to PIC command port

    popa                    # restore registers
    iret                    # return from interrupt
