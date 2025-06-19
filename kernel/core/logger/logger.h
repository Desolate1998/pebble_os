#pragma once
#include <stddef.h>
#include "../../lib/std/string/string.h"

namespace logger
{
    enum class Level
    {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        NONE
    };

    void set_level(Level level);
    void init();

    void info(const std::String& message, const std::String& caller = {});
    void raw(const std::String& message, const std::String& caller = {});
    void warn(const std::String& message, const std::String& caller = {});
    void error(const std::String& message, const std::String& caller = {});
    void debug(const std::String& message, const std::String& caller = {});

    void hex_dump(const void* data, size_t size, const std::String& caller = {});
}
