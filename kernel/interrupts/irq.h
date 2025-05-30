#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>

void irq_install_handlers();
void irq_install_handler(int irq, void (*handler)(void));
void irq_uninstall_handler(int irq);

#endif
