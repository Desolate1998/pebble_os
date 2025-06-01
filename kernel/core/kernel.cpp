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

  // Mask ALL hardware interrupts initially
  asm volatile("outb %0, %1" : : "a"((uint8_t)0xFF), "Nd"((uint16_t)0x21));
  asm volatile("outb %0, %1" : : "a"((uint8_t)0xFF), "Nd"((uint16_t)0xA1));

  logger::INFO("PIC initialized, all hardware interrupts masked");

  // Initialize keyboard interrupt handler
  logger::INFO("Initializing keyboard...");
  keyboard::init();
  logger::INFO("Keyboard initialized");

  logger::INFO("About to enable interrupts...");
  asm("sti");
  logger::INFO("Interrupts enabled - kernel running");

  // Enable keyboard interrupt (IRQ1) - unmask bit 1
  logger::INFO("Enabling keyboard interrupt...");
  asm volatile("outb %0, %1" : : "a"((uint8_t)0xFD), "Nd"((uint16_t)0x21)); // 0xFD = 11111101 (unmask IRQ1)
  logger::INFO("Keyboard interrupt enabled");

  while (true) {
    asm("hlt");
  }
}