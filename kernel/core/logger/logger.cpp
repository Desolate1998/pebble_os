#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include "../../drivers/terminal/terminal.h"
#include "../../constants/color/color.h"
#include "std.h"
#include "logger.h"

namespace logger
{
    Level current_level = Level::INFO;

    void set_level(Level level)
    {
        current_level = level;
    }

    void init()
    {
        terminal::init();
        terminal::set_color(color::light_gray, color::black);
    }

    void print_tag(const char *tag, uint8_t fg_color)
    {
        terminal::set_color(fg_color, color::black);
        terminal::print("[");
        terminal::print(tag);
        terminal::print("] ");
        terminal::set_color(color::light_gray, color::black);
    }

    static void print_message(const std::String &message)
    {
        const char *msg = message.c_str();
        if (!msg || msg[0] == '\0')
            return;

        for (int i = 0; msg[i] != '\0'; ++i)
        {
            terminal::print_char(msg[i]);
        }
    }

    static void print_caller(const std::String &caller)
    {
        const char *tag = caller.c_str();
        if (!tag || tag[0] == '\0')
            return;

        terminal::set_color(color::yellow, color::black);
        terminal::print("[");
        terminal::print(tag);
        terminal::print("]");
        terminal::set_color(color::light_gray, color::black);
    }

    void info(const std::String &message, const std::String &caller)
    {
        if (static_cast<int>(current_level) > static_cast<int>(Level::INFO))
            return;

        print_tag("INF", color::green);
        print_caller(caller);
        terminal::print(" ");
        print_message(message);
        terminal::print_char('\n');
    }

    void raw(const std::String &message, const std::String &caller)
    {

        print_caller(caller);
        terminal::print(" ");
        print_message(message);
        terminal::print_char('\n');
    }

    void warn(const std::String &message, const std::String &caller)
    {
        if (static_cast<int>(current_level) > static_cast<int>(Level::WARN))
            return;

        print_tag("WRN", color::yellow);
        print_caller(caller);
        terminal::print(" ");
        print_message(message);
        terminal::print_char('\n');
    }

    void error(const std::String &message, const std::String &caller)
    {
        if (current_level == Level::NONE)
            return;

        print_tag("ERR", color::red);
        print_caller(caller);
        terminal::print(" ");
        print_message(message);
        terminal::print_char('\n');
    }

    void debug(const std::String &message, const std::String &caller)
    {
        if (static_cast<int>(current_level) > static_cast<int>(Level::DEBUG))
            return;

        print_tag("DBG", color::light_gray);
        print_caller(caller);
        terminal::print(" ");
        print_message(message);
        terminal::print_char('\n');
    }

    void hex_dump(const void *data, size_t size, const std::String &caller)
    {
        if (static_cast<int>(current_level) > static_cast<int>(Level::INFO))
            return;

        print_tag("HEX", color::cyan);
        print_caller(caller);
        terminal::print(" ");

        const uint8_t *bytes = reinterpret_cast<const uint8_t *>(data);
        for (size_t i = 0; i < size; ++i)
        {
            char hex[3];
            const char *hex_chars = "0123456789ABCDEF";
            hex[0] = hex_chars[(bytes[i] >> 4) & 0xF];
            hex[1] = hex_chars[bytes[i] & 0xF];
            hex[2] = '\0';

            terminal::print(hex);
            terminal::print(" ");

            if ((i + 1) % 16 == 0)
                terminal::print("\n");
        }
        terminal::print_char('\n');
    }
}
