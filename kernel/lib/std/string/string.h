#pragma once

namespace std
{
	struct String
	{
		char buffer[256];
		int length;

		String();
		String(const char *str);

		void copy(const char *str);
		void append(const char *str);
		bool equals(const char *str) const;
		bool equals(const String &other) const;
		void clear();

		const char *c_str() const;

		static int strlen(const char *str);
		static bool strcmp(const char *a, const char *b);
		static const char *strchr(const char *str, char c);
	};
}
