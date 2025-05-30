#ifndef IDT_H
#define IDT_H

#include <stdint.h>

struct IDTEntry {
  uint16_t base_low;
  uint16_t sel;
  uint8_t  always0;
  uint8_t  flags;
  uint16_t base_high;
} __attribute__((packed));

struct IDTPointer {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed));

extern "C" void idt_flush(uint32_t);

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void idt_init();

#endif
