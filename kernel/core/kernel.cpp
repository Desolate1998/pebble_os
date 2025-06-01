#include "logger/logger.h"
#include "../interrupts/idt/idt.h"
#include "../drivers/keyboard/keyboard.h"

extern "C" void kernel_main() {
  logger::init();
  logger::set_level(logger::Level::INFO);
  logger::INFO("Kernel started");

  interrupts::init_idt();
  interrupts::init_pic();

  keyboard::init();

  asm volatile("sti");

  logger::INFO("Interrupts enabled");

  while (true) {
    asm volatile("hlt");
  }
}
