#include <iostream>

#include "Gui.h"
#include <thread>

using namespace myApp::gui;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR arguments, int commandShow)
{
    //create gui
    auto gui = Gui();
    gui.createWindow("My App", "MyAppGui");
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
    }

    gui.destroyImGui();
    gui.destroyDevice();
    gui.destroyWindow();

    return EXIT_SUCCESS;
}