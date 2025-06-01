.global load_idt
.global default_interrupt_handler

load_idt:
    movl 4(%esp), %eax
    lidt (%eax)
    ret

.type default_interrupt_handler, @function
default_interrupt_handler:
    pusha
    
    # For now, let's make a simple handler that doesn't send EOI
    # This prevents issues with exceptions and software interrupts
    
    popa
    iret