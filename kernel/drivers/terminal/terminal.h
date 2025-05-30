#pragma once
#include <stdint.h>
#include "constants/color/color.h"

namespace terminal {
	void init();
	void set_color(uint8_t fg, uint8_t bg);
	void print_char(char c);
	void print(const char message[]);
	void print_line(const char message[]);
}