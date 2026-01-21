#include "logger/logger.h"
#include "../drivers/keyboard/keyboard.h"
#include "../interrupts/idt/idt.h"
#include "../lib/std/std.h"
#include "../drivers/terminal/terminal.h"
#include <stddef.h>
extern "C" void *memcpy(void *dest, const void *src, size_t n) {
    unsigned char *d = (unsigned char*)dest;
    const unsigned char *s = (const unsigned char*)src;
    for (size_t i = 0; i < n; i++)
        d[i] = s[i];
    return dest;
}

extern "C" void *memset(void *ptr, int value, size_t n) {
    unsigned char *p = (unsigned char*)ptr;
    for (size_t i = 0; i < n; i++)
        p[i] = (unsigned char)value;
    return ptr;
}

extern "C" int memcmp(const void *s1, const void *s2, size_t n) {
    const unsigned char *a = (const unsigned char*)s1;
    const unsigned char *b = (const unsigned char*)s2;
    for (size_t i = 0; i < n; i++) {
        if (a[i] != b[i]) return a[i] - b[i];
    }
    return 0;
}
extern "C" {
  int handle_command(const char* ptr, size_t len);
}
namespace main
{

  void kernel_initialization()
  {
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

    extern "C" void kernel_main()
  {
    kernel_initialization();

    while (true)
    {
      std::String command = std::get_input();
        // Let Rust handle the command first. If handled, skip C++ handling.
        int handled = handle_command(command.c_str(), (size_t)command.length());
        if (handled) {
            continue;
        }

        if (command.equals("clear"))
        {
          terminal::clear();
        }
    }

    while (true)
    {
      asm volatile("hlt");
    }
  }
}
