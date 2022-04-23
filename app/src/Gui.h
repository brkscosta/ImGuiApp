#pragma once
#include <d3d9.h>




namespace myApp::gui
{
class Gui
{
public:
    Gui();
    ~Gui() = default;

    //hadle window desctuction
    void createWindow(const char* windowName, const char* className) noexcept;
    void destroyWindow() noexcept;

    bool createDevice() noexcept;
    static void resetDevice() noexcept;
    void destroyDevice() noexcept;

    void createImGui() noexcept;
    void destroyImGui() noexcept;

    void beginRender() noexcept;
    void endRender() noexcept;
    void render() noexcept;

    void setWindow(HWND window) { m_window = window; };
    static HWND getWindow() { return m_window; }

    void setWindowClass(WNDCLASSEXA windowClass);

    PDIRECT3D9 getDirect3D9() { return m_d3d;}
    void setDirect3D9(PDIRECT3D9 d3d) { m_d3d = d3d; };

    void setPosition(const POINTS position);

    void setDevice(LPDIRECT3DDEVICE9 device) { m_device = device; }
    static LPDIRECT3DDEVICE9 getDevice() { return m_device; }

    bool exit() const { return m_exit; }
    void setExit(const bool exit) { m_exit = exit; }

    static LRESULT CALLBACK windowProcess(HWND hwnd, UINT uint, WPARAM wparam, LRESULT long_);

private:
    bool                                m_exit;
    inline static HWND                  m_window;
    WNDCLASSEXA                         m_windowClass;
    inline static POINTS                m_position;
    PDIRECT3D9                          m_d3d;
    inline static LPDIRECT3DDEVICE9     m_device;
    inline static D3DPRESENT_PARAMETERS m_presentParameters;
};
}
