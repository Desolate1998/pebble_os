#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

namespace input
{
    enum class EventType
    {
        KeyPress,
        KeyRelease
    };

    struct InputEvent
    {
        EventType type;
        uint8_t scancode;
        char ascii;
        bool shift;
        bool ctrl;
        bool alt;
        bool caps_lock;
        bool enter;
        bool backspace;
    };

    using InputEventHandler = void (*)(const InputEvent &);

    void register_input_handler(InputEventHandler handler);
    void dispatch_event(const InputEvent &event);
}

#endif
