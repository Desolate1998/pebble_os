.global irq0
.global irq1
.global irq2
.global irq3
.global irq4
.global irq5
.global irq6
.global irq7
.global irq8
.global irq9
.global irq10
.global irq11
.global irq12
.global irq13
.global irq14
.global irq15

.macro IRQ_STUB irq_num
irq\irq_num:
    cli                 # Disable interrupts
    pushal              # Push all general-purpose registers
    push $irq_num       # Push IRQ number as argument
    call irq_handler_main # Call C IRQ handler
    add $4, %esp        # Clean up the stack (pop IRQ number)
    popal               # Restore registers
    sti                 # Enable interrupts
    iret                # Return from interrupt
.endm

IRQ_STUB 0
IRQ_STUB 1
IRQ_STUB 2
IRQ_STUB 3
IRQ_STUB 4
IRQ_STUB 5
IRQ_STUB 6
IRQ_STUB 7
IRQ_STUB 8
IRQ_STUB 9
IRQ_STUB 10
IRQ_STUB 11
IRQ_STUB 12
IRQ_STUB 13
IRQ_STUB 14
IRQ_STUB 15
