#include <iostream>

#include "Gui.h"
#include <thread>

using namespace myApp::gui;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR arguments, int commandShow)
{
    //create gui
    auto gui = Gui("My App", "MyAppGui");

    if (!gui.createDevice())
    {
        std::cout << "Device not created" << std::endl;
        return EXIT_FAILURE;
    }

    gui.createImGui();

    while (!gui.exit())
    {
        gui.beginRender();
        gui.render();
        gui.endRender();
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    gui.destroyImGui();
    gui.destroyDevice();
    gui.destroyWindow();

    return EXIT_SUCCESS;
}