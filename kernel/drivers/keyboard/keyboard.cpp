#include "../terminal/terminal.h"
#include "../../interrupts/idt/idt.h"
#include "../../core/logger/logger.h"

extern "C" void keyboard_handler();

extern "C" void keyboard_interrupt_received(uint8_t scancode) {
  static bool extended = false;

  if (scancode == 0xE0) {
    extended = true;
    return;
  }

  if (scancode & 0x80) return; // Ignore key releases

  if (extended) {
    switch (scancode) {
    case 0x48: terminal::print("[UP]"); break;
    case 0x50: terminal::print("[DOWN]"); break;
    case 0x4B: terminal::print("[LEFT]"); break;
    case 0x4D: terminal::print("[RIGHT]"); break;
    default: terminal::print("[EXT]");
    }
    extended = false;
    return;
  }

  static const char scancode_to_ascii[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\','z',
    'x','c','v','b','n','m',',','.','/', 0,'*',0,' ',0,0,0
  };

  char c = 0;
  if (scancode < 128)
    c = scancode_to_ascii[scancode];

  if (c) {
    char str[2] = { c, '\0' };
    terminal::print(str);
  }
}

namespace keyboard {
  void init() {
    interrupts::set_idt_entry(33, keyboard_handler);
  }
}
