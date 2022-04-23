#include <iostream>

#include "Gui.h"
#include <thread>

using namespace myApp::gui;

int WINAPI WinMain([[maybe_unused]] HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    [[maybe_unused]] LPSTR arguments,
    [[maybe_unused]] int commandShow)
{
    //create gui
    auto gui = Gui("My App", "MyAppGui");

    while (gui.exit())
    {
        gui.beginRender();
        gui.render();
        gui.endRender();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return gui.destroyApp();;
}