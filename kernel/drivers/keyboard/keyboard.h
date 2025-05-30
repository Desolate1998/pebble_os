#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

namespace keyboard {

	void initialize();
	uint8_t read_scancode();
	bool is_key_pressed();
	char get_char();
	void handle_interrupt(uint8_t scancode);

}

#endif
