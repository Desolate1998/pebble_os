#include "irq.h"
#include "pic.h"

static void* irq_routines[16] = { 0 };

extern "C" void irq_handler_main(int irq_num);

void irq_install_handler(int irq, void (*handler)(void)) {
  irq_routines[irq] = (void*)handler;
}

void irq_uninstall_handler(int irq) {
  irq_routines[irq] = 0;
}

void irq_install_handlers() {
  pic_remap(0x20, 0x28);
  // Setup IDT gates here for IRQs 0-15, e.g.:
  // idt_set_gate(0x20 + irq, (uint32_t)irq_stub_address, 0x08, 0x8E);
}

extern "C" void irq_handler_main(int irq_num) {
  if (irq_routines[irq_num]) {
    ((void(*)())irq_routines[irq_num])();
  }
  pic_send_eoi(irq_num);
}
