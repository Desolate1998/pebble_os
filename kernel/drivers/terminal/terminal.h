#pragma once
#include <stddef.h>
#include <stdint.h>

namespace terminal {
	void init();
	void set_color(uint8_t color);
	void put_at(char c, uint8_t color, size_t x, size_t y);
	void put_char(char c);
	void write(const char* data, size_t size);
	void write_string(const char* data);
}
