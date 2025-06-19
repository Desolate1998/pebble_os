.section .multiboot
.align 4
.long 0x1BADB002                
.long 0                         
.long -(0x1BADB002 + 0)         

.section .bss
    .lcomm stack, 16384         

.section .text
.global _start
.extern kernel_main
.extern gdt_install             

_start:
    call gdt_install           
    mov $stack + 16384, %esp   
    call kernel_main           

hang:
    cli
    hlt
    jmp hang
