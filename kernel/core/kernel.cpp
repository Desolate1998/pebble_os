#include "logger/logger.h"
#include "drivers/keyboard/keyboard.h"
#include "../interrupts/idt/idt.h"
#include "../drivers/keyboard/keyboard.h"

extern "C" void kernel_main() {
  logger::init();
  logger::set_level(logger::Level::INFO);
  logger::INFO("Kernel started");

  logger::INFO("About to initialize IDT...");
  interrupts::init_idt();
  logger::INFO("IDT initialized successfully");

  logger::INFO("About to initialize PIC...");
  interrupts::init_pic();

  asm volatile("outb %0, %1" : : "a"((uint8_t)0xFF), "Nd"((uint16_t)0x21));
  asm volatile("outb %0, %1" : : "a"((uint8_t)0xFF), "Nd"((uint16_t)0xA1));

  logger::INFO("PIC initialized, all hardware interrupts masked");

  logger::INFO("About to enable interrupts...");
  asm("sti");
  logger::INFO("Interrupts enabled - kernel running");

  logger::INFO("Testing interrupt system...");

  for (int i = 0; i < 1000000; i++) {
    asm("nop");
  }

  logger::INFO("Interrupt test passed - no crashes");

  while (true) {
    asm("hlt");
  }
}