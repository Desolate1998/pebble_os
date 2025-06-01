#pragma once
#include <stdint.h>

namespace interrupts {
  void init_idt();
  void init_pic();
  void set_idt_entry(int index, void (*handler)());
}
