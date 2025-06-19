#include "terminal.h"
#include "constants/color/color.h"
#include "../keyboard/input.h"
#include "constants/keys/key.h"
#include "std.h"

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

	
	void handle_input(const input::InputEvent &e)
	{
		if (e.type == input::EventType::KeyPress)
		{
			if (e.enter)
			{
				disable_input_mode();
				print_char('\n');
			}
			else if (e.ascii)
			{
				if (input_mode)
				{
					input_buffer.append(&e.ascii);
					print_char(e.ascii);
				}
			}
			else
			{
				switch (e.scancode)
				{
				case 0x48:
					scroll_up();
					break;
				case 0x50:
					scroll_down();
					break;
				}
			}
		}
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
		current_read_row_index--;
		auto_follow = false;
		render();
	}

	void scroll_down()
	{
		int max_scroll = (current_write_row_index >= (display_height - 1)) ? current_write_row_index - (display_height - 1) : 0;

		current_read_row_index++;
		auto_follow = false;
		render();
	}

	void enable_input_mode()
	{
		input_mode = true;
	}

	void disable_input_mode()
	{
		input_mode = false;
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
