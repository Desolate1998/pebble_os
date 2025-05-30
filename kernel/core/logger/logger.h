#pragma once

namespace logger {
	enum class Level {
		INFO,
		WARN,
		ERROR,
		NONE
	};

	void INFO(const char message[]);
	void WARN(const char message[]);
	void ERROR(const char message[]);
	void init();
	void set_level(Level level);
}
