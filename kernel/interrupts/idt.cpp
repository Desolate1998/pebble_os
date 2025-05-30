#include "idt.h"

static IDTEntry idt_entries[256];
static IDTPointer idt_ptr;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
  idt_entries[num].base_low = base & 0xFFFF;
  idt_entries[num].base_high = (base >> 16) & 0xFFFF;
  idt_entries[num].sel = sel;
  idt_entries[num].always0 = 0;
  idt_entries[num].flags = flags;
}

extern "C" void idt_flush(uint32_t);

void idt_init() {
  idt_ptr.limit = sizeof(IDTEntry) * 256 - 1;
  idt_ptr.base = (uint32_t)&idt_entries;

  for (int i = 0; i < 256; i++) {
    idt_set_gate(i, 0, 0, 0);
  }

  idt_flush((uint32_t)&idt_ptr);
}

extern "C" void idt_flush(uint32_t idt_ptr_addr) {
  asm volatile("lidt (%0)" : : "r"(idt_ptr_addr));
}
