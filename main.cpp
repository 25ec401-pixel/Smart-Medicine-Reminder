// Smart Medicine Reminder - Console version
// Standard C++17, no external libraries.

#include <exception>
#include <iostream>

#include "app.h"
#include "utils.h"

int main()
{
    try
    {
        ConsoleApp app;
        app.run();
    }
    catch (const InputClosed &)
    {
        // The keyboard input ended (for example Ctrl+Z, Enter on Windows).
        std::cout << "\n\nInput closed. Your data is already saved. Goodbye!\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "\nUnexpected error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
