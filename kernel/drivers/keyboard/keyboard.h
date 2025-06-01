#pragma once
#include <stdint.h>

namespace keyboard {
  void init();
}

extern "C" void keyboard_interrupt_received(uint8_t scancode);
