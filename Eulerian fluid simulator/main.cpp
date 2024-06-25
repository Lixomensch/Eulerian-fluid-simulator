#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include "main_window.h"

int main()
{
    main_window window;
    window.run_window();
    window.~main_window();
    return 0;
}