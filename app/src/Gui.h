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
    void createWindow(const char* windowName, const char* className) noexcept;
    void destroyWindow() noexcept;

    bool createDevice() noexcept;
    void resetDevice() noexcept;
    void destroyDevice() noexcept;

    void createImGui() noexcept;
    void destroyImGui() noexcept;

    void beginRender() noexcept;
    void endRender() noexcept;
    void render() noexcept;

    void setWindow(HWND window);

    void settWindowClass(WNDCLASSEXA windowClass);

    void setDirect3D9(PDIRECT3D9 d3d);

    void setPosition(const POINTS position);

    void setPresentParameters(D3DPRESENT_PARAMETERS presentParameters);

    bool exit() const { return m_exit; }
    void setExit(const bool exit) { m_exit = exit; }



private:
    bool                  m_exit;

};

LRESULT WINAPI windowProcess(HWND hwnd, UINT uint, WPARAM wparam, LRESULT long_);
}
