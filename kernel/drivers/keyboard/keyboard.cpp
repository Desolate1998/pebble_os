#include "keyboard.h"

namespace keyboard {

  static volatile uint8_t last_scancode = 0;
  static volatile bool key_pressed = false;

  void initialize() {
  }

  uint8_t read_scancode() {
    uint8_t sc = last_scancode;
    last_scancode = 0;
    key_pressed = false;
    return sc;
  }

  bool is_key_pressed() {
    return key_pressed;
  }

  char get_char() {
    switch (last_scancode) {
    case 0x1E: return 'a';
    case 0x30: return 'b';
    case 0x2E: return 'c';
    case 0x20: return 'd';
    case 0x12: return 'e';
    default: return 0;
    }
  }

  void handle_interrupt(uint8_t scancode) {
    last_scancode = scancode;
    key_pressed = true;
  }

}
