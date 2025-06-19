#include "input.h"
#include "../../core/logger/logger.h"

namespace input
{
    constexpr int MAX_INPUT_HANDLERS = 255;
    static InputEventHandler handlers[MAX_INPUT_HANDLERS];
    static int handler_count = 0;

    void register_input_handler(InputEventHandler handler)
    {
        if (handler_count < MAX_INPUT_HANDLERS)
        {
            handlers[handler_count++] = handler;
        }
        else
        {
            logger::warn("Max input handlers reached");
        }
    }

    void dispatch_event(const InputEvent &event)
    {
        for (int i = 0; i < handler_count; ++i)
        {
            handlers[i](event);
        }
    }
}
