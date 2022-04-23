#pragma once
#include <d3d9.h>

namespace myApp::gui
{
class Gui
{
public:
    Gui(const char* windowName, const char* className);
    ~Gui() = default;

    //hadle window desctuction

    static int destroyApp();

    static void beginRender() noexcept;
    static void endRender() noexcept;
    void render() noexcept;

    void setWindowClass(WNDCLASSEXA windowClass);

    void setPosition(const POINTS position);

    bool exit() const { return m_exit; }
    void setExit(const bool exit) { m_exit = exit; }

private:
    static void destroyWindow() noexcept;
    static void destroyDevice() noexcept;
    static void destroyImGui() noexcept;
    static void createImGui() noexcept;
    static bool createDevice() noexcept;
    static void createWindow(const char* windowName, const char* className) noexcept;

    bool                                m_exit;

};

    inline HWND                  window;
    inline WNDCLASSEXA           windowClass;
    inline POINTS                position;
    inline PDIRECT3D9            d3d;
    inline LPDIRECT3DDEVICE9     device;
    inline D3DPRESENT_PARAMETERS presentParameters;

    void resetDevice() noexcept;
    LRESULT CALLBACK windowProcess(HWND hwnd, UINT uint, WPARAM wparam, LRESULT long_);
}
