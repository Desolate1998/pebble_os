#include "logger/logger.h"
#include "drivers/keyboard/keyboard.h"
#include "interrupts/pic.h"
#include "interrupts/idt.h"
#include "interrupts/irq.h"

extern "C" void kernel_main() {
  pic_remap(0x20, 0x28);
  idt_init();
  irq_install_handlers();
  asm volatile ("sti");

  logger::init();
  logger::ERROR("SYSTEM Loaded");
  logger::WARN("SYSTEM Loaded");
  logger::INFO("SYSTEM Loaded");

  //keyboard::initialize();
  while (true) {

    //asm volatile ("hlt"); // halt CPU until next interrupt
  }

}

