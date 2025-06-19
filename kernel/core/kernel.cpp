#include "logger/logger.h"
#include "../drivers/keyboard/keyboard.h"
#include "../interrupts/idt/idt.h"
#include "../lib/std/std.h"

namespace main {

  void kernel_initialization() {
    logger::init();
    logger::set_level(logger::Level::INFO);

    logger::info("Kernel started", "KERNEL");

    logger::info("Initializing IDT...", "KERNEL");
    interrupts::init_idt();
    logger::info("IDT initialized", "KERNEL");

    logger::info("Initializing PIC...", "KERNEL");
    interrupts::init_pic();
    logger::info("PIC initialized", "KERNEL");

    logger::info("Initializing keyboard driver...", "KERNEL");
    keyboard::init();
    logger::info("Keyboard driver initialized", "KERNEL");
    
    asm volatile("sti");

  }

  extern "C" void kernel_main() {
    kernel_initialization();
while (true)
{
  /* code */
  
  logger::info("Please enter username");
  std::String username = std::get_input();
  logger::info("your username is");
  logger::info(username);
}


    while (true) {
      asm volatile("hlt");
    }
  }
}
