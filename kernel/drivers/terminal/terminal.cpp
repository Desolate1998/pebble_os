// terminal.cpp
#include "terminal.h"
#include "constants/color/color.h"
#include "../keyboard/input.h"
#include "constants/keys/key.h"
#include "std.h"

static inline void outb(uint16_t port, uint8_t val)
{
	asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
	uint8_t ret;
	asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

namespace terminal
{
	struct character
	{
		char character;
		uint8_t color;
	};

	const uint8_t TERMINAL_BUFFER_MAX_HEIGHT = 200;
	const uint8_t TERMINAL_BUFFER_MAX_WIDTH = 200;
	const uint8_t display_width = 80;
	const uint8_t display_height = 25;
	constexpr auto TERMINAL_BUFFER_MEMORY_ADDRESS = 0xB8000;

	uint16_t *const terminal_buffer = (uint16_t *)TERMINAL_BUFFER_MEMORY_ADDRESS;
	character terminal_buffer_characters[TERMINAL_BUFFER_MAX_HEIGHT][TERMINAL_BUFFER_MAX_WIDTH];
	uint8_t current_column = 0;
	int8_t current_read_row_index = 0;
	uint8_t current_write_row_index = 0;
	uint8_t current_color = ((uint8_t)color::light_gray) | ((uint8_t)color::black << 4);
	bool auto_follow = true;
	bool input_mode = false;
	std::String input_buffer;

	uint8_t cursor_row = 0;
	uint8_t cursor_column = 0;

	void update_cursor()
	{
		uint16_t cursor_pos = (cursor_row * display_width) + cursor_column;
		outb(0x3D4, 0x0F);
		outb(0x3D5, cursor_pos & 0xFF);
		outb(0x3D4, 0x0E);
		outb(0x3D5, (cursor_pos >> 8) & 0xFF);
	}

	void hide_cursor()
	{
		outb(0x3D4, 0x0A);
		outb(0x3D5, 0x20);
	}

	void show_cursor()
	{
		outb(0x3D4, 0x0A);
		uint8_t val = inb(0x3D5);
		val &= ~0x20;
		outb(0x3D5, val);
	}

	void clamp_cursor()
	{
		if (cursor_column >= display_width)
			cursor_column = display_width - 1;
		if (cursor_row >= display_height)
			cursor_row = display_height - 1;
		if (cursor_column < 0)
			cursor_column = 0;
		if (cursor_row < 0)
			cursor_row = 0;
	}

	void update_cursor_from_terminal()
	{
		cursor_row = current_write_row_index - current_read_row_index;
		cursor_column = current_column;
		clamp_cursor();
	}

	void handle_input(const input::InputEvent &e)
	{
		if (e.type == input::EventType::KeyPress)
		{
			if (e.enter)
			{
				if (e.shift)
				{
					input_buffer.append("\n");
					print_char('\n');
				}
				else
				{
					disable_input_mode();
					print_char('\n');
				}
				update_cursor_from_terminal();
				update_cursor();
			}
			else if (e.backspace)
			{
				if (input_mode && input_buffer.length() != 0)
				{
					input_buffer.pop();
					remove_last_char();
					update_cursor_from_terminal();
					update_cursor();
				}
			}
			else if (e.ascii)
			{
				if (input_mode)
				{
					char temp_str[2] = {e.ascii, '\0'};
					input_buffer.append(temp_str);
					print_char(e.ascii);
					update_cursor_from_terminal();
					update_cursor();
				}
			}
			else
			{
				switch (e.scancode)
				{
				case 0x48: // up arrow
					scroll_up();
					break;
				case 0x50: // down arrow
					scroll_down();
					break;
				}
			}
		}
	}

	void clear()
	{
		clear_input_buffer();

		for (uint8_t row = 0; row < TERMINAL_BUFFER_MAX_HEIGHT; row++)
		{
			for (uint8_t col = 0; col < TERMINAL_BUFFER_MAX_WIDTH; col++)
			{
				terminal_buffer_characters[row][col].character = ' ';
				terminal_buffer_characters[row][col].color = current_color;
			}
		}

		current_column = 0;
		current_read_row_index = 0;
		current_write_row_index = 0;

		cursor_column = 0;
		cursor_row = 0;

		auto_follow = true;

		render();
	}

	const std::String &get_input_buffer()
	{
		return input_buffer;
	}

	void clear_input_buffer()
	{
		input_buffer.clear();
	}

	bool is_input_mode_enabled()
	{
		return input_mode;
	}

	void init()
	{
		input::register_input_handler(handle_input);
		current_color = ((uint8_t)color::light_gray) | ((uint8_t)color::black << 4);
		hide_cursor();
	}

	void set_color(uint8_t fg, uint8_t bg)
	{
		current_color = fg | (bg << 4);
	}

	void render()
	{
		for (uint8_t row = 0; row < display_height; row++)
		{
			for (uint8_t column = 0; column < display_width; column++)
			{
				uint8_t buffer_row = current_read_row_index + row;
				terminal_buffer[row * display_width + column] =
					(uint16_t)terminal_buffer_characters[buffer_row][column].character |
					(terminal_buffer_characters[buffer_row][column].color << 8);
			}
		}
		if (input_mode)
		{
			update_cursor_from_terminal();
			show_cursor();
			update_cursor();
		}
		else
		{
			hide_cursor();
		}
	}

	void overflow_handler()
	{
		for (uint8_t i = 1; i < TERMINAL_BUFFER_MAX_HEIGHT; i++)
		{
			for (uint8_t j = 0; j < TERMINAL_BUFFER_MAX_WIDTH; j++)
			{
				terminal_buffer_characters[i - 1][j] = terminal_buffer_characters[i][j];
			}
		}
	}

	void scroll_up()
	{
		if (current_read_row_index > 0)
		{
			current_read_row_index--;
			auto_follow = false;
			render();
		}
	}

	void scroll_down()
	{
		int max_scroll = (current_write_row_index >= (display_height - 1)) ? current_write_row_index - (display_height - 1) : 0;
		if (current_read_row_index < max_scroll)
		{
			current_read_row_index++;
			auto_follow = false;
			render();
		}
	}

	void enable_input_mode()
	{
		input_mode = true;
		update_cursor_from_terminal();
		show_cursor();
		update_cursor();
	}

	void disable_input_mode()
	{
		input_mode = false;
		hide_cursor();
	}

	void remove_last_char()
	{
		if (current_column > 0)
		{
			current_column--;
		}
		else if (current_write_row_index > 0)
		{
			current_write_row_index--;
			current_column = display_width - 1;
		}
		else
		{
			return;
		}

		terminal_buffer_characters[current_write_row_index][current_column].character = ' ';
		terminal_buffer_characters[current_write_row_index][current_column].color = current_color;
		render();
	}

	void print_char(char c)
	{
		auto_follow = true;

		if (c == '\n')
		{
			current_column = 0;
			current_write_row_index++;
			current_read_row_index++;
		}
		else
		{
			terminal_buffer_characters[current_write_row_index][current_column].character = c;
			terminal_buffer_characters[current_write_row_index][current_column].color = current_color;
			current_column++;

			if (current_column >= display_width)
			{
				current_column = 0;
				current_write_row_index++;
			}
		}

		if (current_write_row_index >= TERMINAL_BUFFER_MAX_HEIGHT)
		{
			overflow_handler();
			current_write_row_index = TERMINAL_BUFFER_MAX_HEIGHT - 1;
		}

		if (auto_follow)
		{
			int max_scroll = (current_write_row_index >= (display_height - 1)) ? current_write_row_index - (display_height - 1) : 0;
			if (current_read_row_index < max_scroll)
				current_read_row_index = max_scroll;
			if (current_read_row_index > max_scroll)
				current_read_row_index = max_scroll;
			if (current_read_row_index < 0)
				current_read_row_index = 0;
		}

		render();
	}

	void print(const char message[])
	{
		auto_follow = true;
		for (int i = 0; message[i] != '\0'; i++)
		{
			print_char(message[i]);
		}
	}

	void print_line(const char message[])
	{
		auto_follow = true;
		print(message);
		print_char('\n');
	}
}
