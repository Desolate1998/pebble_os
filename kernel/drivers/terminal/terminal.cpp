#include "terminal.h"
#include "constants/color/color.h"

constexpr auto TERMINAL_BUFFER_MEMORY_ADDRESS = 0xB8000;
uint16_t* const terminal_buffer = (uint16_t*)TERMINAL_BUFFER_MEMORY_ADDRESS;

const uint8_t width = 80;
const uint8_t height = 25;

uint8_t current_row = 0;
uint8_t current_column = 0;
uint8_t current_color = ((uint8_t)color::light_gray) | ((uint8_t)color::black<< 4);

void terminal::init() {
	current_row = 0;
	current_column = 0;
	current_color = ((uint8_t)color::light_gray) | ((uint8_t)color::black << 4);
}

void terminal::set_color(uint8_t fg, uint8_t bg) {
	current_color = fg | (bg << 4);
}

void terminal::print_char(char c) {
	if (c == '\n') {
		current_column = 0;
		current_row++;
		if (current_row >= height) {
			current_row = 0;
		}
		return;
	}

	terminal_buffer[current_row * width + current_column] = (uint16_t)c | (current_color << 8);

	current_column++;
	if (current_column >= width) {
		current_column = 0;
		current_row++;
		if (current_row >= height) {
			current_row = 0;
		}
	}
}

void terminal::print(const char message[]) {
	for (int i = 0; message[i] != '\0'; i++) {
		print_char(message[i]);
	}
}

void terminal::print_line(const char message[]) {
	print(message);
	print_char('\n');
}
