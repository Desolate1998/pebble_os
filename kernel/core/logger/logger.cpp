#include "logger.h"
#include "../../drivers/terminal/terminal.h"
#include "../../constants/color/color.h"

namespace logger {
	Level current_level = Level::INFO;

	void set_level(Level level) {
		current_level = level;
	}

	void init() {
		terminal::init();
		terminal::set_color(color::light_gray, color::black);
	}

	void print_tag(const char* tag, uint8_t fg_color) {
		terminal::set_color(fg_color, color::black);
		terminal::print("[");
		terminal::print(tag);
		terminal::print("] ");
		terminal::set_color(color::light_gray, color::black);
	}

	static void print_message(const char message[]) {
		if (message[0] >= 'A' && message[0] <= 'Z') {
			terminal::print_char(message[0]);
		}
		else if (message[0] >= 'a' && message[0] <= 'z') {
			terminal::print_char(message[0] - 32);
		}

		for (int i = 1; message[i] != '\0'; i++) {
			char c = message[i];
			if (c >= 'A' && c <= 'Z') c += 32;
			terminal::print_char(c);
		}
	}

	void INFO(const char message[]) {
		if (static_cast<int>(current_level) > static_cast<int>(Level::INFO)) return;
		print_tag("INF", color::green);
		print_message(message);
		terminal::print_char('\n');
	}

	void WARN(const char message[]) {
		if (static_cast<int>(current_level) > static_cast<int>(Level::WARN)) return;
		print_tag("WRN", color::yellow);
		print_message(message);
		terminal::print_char('\n');
	}

	void ERROR(const char message[]) {
		if (current_level == Level::NONE) return;
		print_tag("ERR", color::red);
		print_message(message);
		terminal::print_char('\n');
	}
}
