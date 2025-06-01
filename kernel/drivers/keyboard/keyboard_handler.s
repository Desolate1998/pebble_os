.global keyboard_handler
.global keyboard_debug_handler
.global keyboard_minimal_handler
.global keyboard_ultra_minimal_handler
.type keyboard_handler, @function
.type keyboard_debug_handler, @function
.type keyboard_minimal_handler, @function
.type keyboard_ultra_minimal_handler, @function

# This does absolutely NOTHING - just returns
keyboard_ultra_minimal_handler:
    iret

keyboard_minimal_handler:
    # Absolute minimal handler - no pusha/popa, no C calls
    inb $0x60, %al          # read and discard scancode
    movb $0x20, %al         # prepare EOI
    outb %al, $0x20         # send EOI
    iret                    # return

keyboard_debug_handler:
    pusha                   # save all general-purpose registers
    
    # Read scancode from keyboard data port (0x60)
    inb $0x60, %al          
    
    # Call C function for debugging
    pushl %eax              # push scancode as parameter
    call keyboard_interrupt_received
    addl $4, %esp           # clean up stack (remove 1 parameter)
    
    # Send End Of Interrupt (EOI) to master PIC
    movb $0x20, %al
    outb %al, $0x20         # send EOI to PIC command port (0x20)
    
    popa                    # restore registers
    iret                    # return from interrupt

keyboard_handler:
    pusha                   # save all general-purpose registers
    
    # Read scancode from keyboard data port (0x60)
    inb $0x60, %al          
    
    # Send End Of Interrupt (EOI) to master PIC
    movb $0x20, %al
    outb %al, $0x20         # send EOI to PIC command port (0x20)
    
    popa                    # restore registers
    iret                    # return from interrupt