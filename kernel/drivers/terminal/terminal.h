#pragma once
#include <stdint.h>
#include "constants/color/color.h"
#include "std.h"

namespace terminal {
	
	void init();
	void set_color(uint8_t fg, uint8_t bg);
	void print_char(char c);
	void print(const char message[]);
	void print_line(const char message[]);
	void scroll_down();
	void scroll_up();
	void enable_input_mode();
	void disable_input_mode();
	bool is_input_mode_enabled();

	const std::String& get_input_buffer();
	void clear_input_buffer();
	void process_input_events();
}
