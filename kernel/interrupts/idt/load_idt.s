.global load_idt
.global default_interrupt_handler

load_idt:
    movl 4(%esp), %eax
    lidt (%eax)
    ret

default_interrupt_handler:
    pusha
    movb $0x20, %al
    outb %al, $0x20
    outb %al, $0xA0
    popa
    iret
