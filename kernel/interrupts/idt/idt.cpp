#include "idt.h"

namespace {
  struct IDTEntry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
  } __attribute__((packed));

  struct IDTPointer {
    uint16_t limit;
    uint32_t base;
  } __attribute__((packed));

  constexpr int IDT_SIZE = 256;
  IDTEntry idt[IDT_SIZE];
  IDTPointer idt_ptr;
  extern "C" void load_idt(uint32_t);
  extern "C" void default_interrupt_handler();

  // Add these port I/O functions
  inline void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
  }

  inline uint8_t inb(uint16_t port) {
    uint8_t result;
    asm volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
  }
}

void interrupts::set_idt_entry(int index, void (*handler)()) {
  uint32_t addr = (uint32_t)handler;
  idt[index].offset_low = addr & 0xFFFF;
  idt[index].selector = 0x08;  // Kernel code segment
  idt[index].zero = 0;
  idt[index].type_attr = 0x8E; // Interrupt gate
  idt[index].offset_high = (addr >> 16) & 0xFFFF;
}

void interrupts::init_idt() {
  // Set all entries to default handler first
  for (int i = 0; i < IDT_SIZE; i++) {
    set_idt_entry(i, default_interrupt_handler);
  }

  idt_ptr.limit = sizeof(IDTEntry) * IDT_SIZE - 1;
  idt_ptr.base = (uint32_t)&idt;
  load_idt((uint32_t)&idt_ptr);
}

void interrupts::init_pic() {
  // Initialize master PIC
  outb(0x20, 0x11); // ICW1: Initialize
  outb(0x21, 0x20); // ICW2: Master PIC vector offset (32)
  outb(0x21, 0x04); // ICW3: Tell master PIC that slave is at IRQ2
  outb(0x21, 0x01); // ICW4: 8086 mode

  // Initialize slave PIC
  outb(0xA0, 0x11); // ICW1: Initialize
  outb(0xA1, 0x28); // ICW2: Slave PIC vector offset (40)
  outb(0xA1, 0x02); // ICW3: Tell slave PIC its cascade identity
  outb(0xA1, 0x01); // ICW4: 8086 mode

  // Unmask all interrupts
  outb(0x21, 0x00);
  outb(0xA1, 0x00);
}