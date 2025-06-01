.global load_idt
.global default_interrupt_handler

load_idt:
    movl 4(%esp), %eax
    lidt (%eax)
    ret

.type default_interrupt_handler, @function
default_interrupt_handler:
    pusha
    
    # Send EOI to master PIC for hardware interrupts
    movb $0x20, %al
    outb %al, $0x20
    
    # Send EOI to slave PIC as well (safe to do always)
    outb %al, $0xA0
    
    popa
    iret