#include <stddef.h>
#include <stdint.h>
#include "drivers/terminal/terminal.h"

extern "C" void kernel_main() {
	terminal::init();

	for (size_t i = 0; i < 10000; i++) {
		terminal::write_string("Hello, kernel World!\n");
	}
}

