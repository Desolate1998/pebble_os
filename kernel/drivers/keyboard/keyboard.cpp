#include "keyboard.h"
#include "../../interrupts/idt/idt.h"
#include "../../core/logger/logger.h"

extern "C" void keyboard_handler();

namespace keyboard {
	void init() {
		interrupts::set_idt_entry(33, keyboard_handler);  // IRQ1
	}
}
