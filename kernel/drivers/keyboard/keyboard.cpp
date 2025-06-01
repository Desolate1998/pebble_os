#include "keyboard.h"
#include "../../interrupts/idt/idt.h"
#include "../../core/logger/logger.h"

extern "C" void keyboard_handler();
extern "C" void keyboard_debug_handler();
extern "C" void keyboard_minimal_handler();
extern "C" void keyboard_ultra_minimal_handler();

extern "C" void keyboard_interrupt_received(uint8_t scancode) {
  logger::INFO("Keyboard interrupt! Scancode received");
}

namespace keyboard {
  void init() {
    logger::INFO("Setting ULTRA MINIMAL keyboard handler at interrupt 33 (IRQ1)");
    interrupts::set_idt_entry(33, keyboard_ultra_minimal_handler);  // Does absolutely nothing
    logger::INFO("Ultra minimal keyboard handler set");
  }
}