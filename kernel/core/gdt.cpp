#include <stdint.h>

struct GDTEntry {
  uint16_t limit_low;
  uint16_t base_low;
  uint8_t base_middle;
  uint8_t access;
  uint8_t granularity;
  uint8_t base_high;
} __attribute__((packed));

struct GDTPtr {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed));

GDTEntry gdt[3];
GDTPtr gdt_ptr;

extern "C" void gdt_flush(uint32_t);

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
  gdt[num].base_low = (base & 0xFFFF);
  gdt[num].base_middle = (base >> 16) & 0xFF;
  gdt[num].base_high = (base >> 24) & 0xFF;

  gdt[num].limit_low = (limit & 0xFFFF);
  gdt[num].granularity = (limit >> 16) & 0x0F;

  gdt[num].granularity |= (gran & 0xF0);
  gdt[num].access = access;
}

extern "C" void gdt_install() {
  gdt_ptr.limit = (sizeof(GDTEntry) * 3) - 1;
  gdt_ptr.base = (uint32_t)&gdt;

  gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
  gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
  gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment

  gdt_flush((uint32_t)&gdt_ptr);
}
