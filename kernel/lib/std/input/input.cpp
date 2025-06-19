#import "./input.h"
#include "../../../drivers/terminal/terminal.h"

namespace std
{
    String get_input()
    {
        terminal::enable_input_mode();
        terminal::clear_input_buffer();


        while (terminal::is_input_mode_enabled())
        {
            
        }

        return terminal::get_input_buffer();
    }
}
