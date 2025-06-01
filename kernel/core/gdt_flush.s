.global gdt_flush
gdt_flush:
    movl 4(%esp), %eax
    lgdt (%eax)
    mov $0x10, %ax     # Data segment selector (2nd entry in GDT)
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    jmp $0x08, $next   # Code segment selector (1st entry after null)

next:
    ret
