#include "../terminal/terminal.h"
#include "../../interrupts/idt/idt.h"
#include "../../core/logger/logger.h"
#include "./input.h"

extern "C" void keyboard_handler();

namespace
{
    bool shift_pressed = false;
    bool ctrl_pressed = false;
    bool alt_pressed = false;
    bool caps_lock_on = false;
}

extern "C" void keyboard_interrupt_received(uint8_t scancode)
{
    static bool extended = false;

    if (scancode == 0xE0)
    {
        extended = true;
        return;
    }

    bool key_release = scancode & 0x80;
    uint8_t pure_scancode = scancode & 0x7F;

    static const char scancode_to_ascii[128] = {
        0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z',
        'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0};

    // Update modifier keys state
    switch (pure_scancode)
    {
    case 0x2A: // Left Shift
    case 0x36: // Right Shift
        shift_pressed = !key_release;
        break;
    case 0x1D: // Ctrl
        ctrl_pressed = !key_release;
        break;
    case 0x38: // Alt
        alt_pressed = !key_release;
        break;
    case 0x3A: // Caps Lock
        if (!key_release) // toggle on key press only
            caps_lock_on = !caps_lock_on;
        break;
    default:
        break;
    }

    char ascii = 0;
    if (!key_release)
    {
        if (pure_scancode < 128)
        {
            ascii = scancode_to_ascii[pure_scancode];

            if (ascii >= 'a' && ascii <= 'z')
            {
                if (shift_pressed ^ caps_lock_on)
                    ascii -= 32; // Convert to uppercase
            }
            else if (shift_pressed)
            {
                // Handle shifted number and symbol keys
                static const char shifted_chars[128] = {
                    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
                    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
                    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|', 'Z',
                    'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0};
                ascii = shifted_chars[pure_scancode];
            }
        }
    }

    input::InputEvent event;
    event.type = key_release ? input::EventType::KeyRelease : input::EventType::KeyPress;
    event.scancode = pure_scancode;
    event.ascii = ascii;
    event.shift = shift_pressed;
    event.ctrl = ctrl_pressed;
    event.alt = alt_pressed;
    event.caps_lock = caps_lock_on;
    event.enter = (pure_scancode == 0x1C);

    input::dispatch_event(event);
}

namespace keyboard
{
    void init()
    {
        interrupts::set_idt_entry(33, keyboard_handler);
    }
}
