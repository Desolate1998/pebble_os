#include "terminal.h"
#include <stddef.h>

extern "C" void terminal_clear() {
    terminal::clear();
}

extern "C" void terminal_print_ptr(const char* ptr, size_t len) {
    if (!ptr || len == 0) return;
    for (size_t i = 0; i < len; ++i) {
        terminal::print_char(ptr[i]);
    }
}
