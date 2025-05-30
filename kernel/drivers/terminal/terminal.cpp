
#include "terminal.h"

enum vga_color {
	black = 0,
	blue = 1,
	green = 2,
	cyan = 3,
	red = 4,
	magenta = 5,
	brown = 6,
	light_grey = 7,
	dark_grey = 8,
	light_blue = 9,
	light_green = 10,
	light_cyan = 11,
	light_red = 12,
	light_magenta = 13,
	light_brown = 14,
	white = 15,
};


static constexpr size_t VGA_WIDTH = 80;
static constexpr size_t VGA_HEIGHT = 25;
static volatile uint16_t* const VGA_MEMORY = (uint16_t*)0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

static uint8_t make_color(uint8_t fg, uint8_t bg) {
	return fg | bg << 4;
}

static uint16_t make_vga_entry(char c, uint8_t color) {
	return (uint16_t)c | (uint16_t)color << 8;
}

void terminal::init() {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = make_color(vga_color::light_grey, vga_color::black);
	terminal_buffer = (uint16_t*)VGA_MEMORY;

	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = make_vga_entry(' ', terminal_color);
		}
	}
}

void terminal::set_color(uint8_t color) {
	terminal_color = color;
}

void terminal::put_at(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = make_vga_entry(c, color);
}

void terminal::put_char(char c) {
	if (c == '\n') {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
		return;
	}

	put_at(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}
}

void terminal::write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		put_char(data[i]);
}

void terminal::write_string(const char* data) {
	while (*data)
		put_char(*data++);
}

