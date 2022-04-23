#include "Gui.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx9.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace myApp::gui;

constexpr int WIDTH = 500;
constexpr int HEIGHT = 300;

Gui::Gui():
    m_exit(false)
{}

LRESULT CALLBACK Gui::windowProcess(const HWND hWnd, const UINT msg, WPARAM wParam, const LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;

    switch (msg)
    {
    case WM_SIZE:
        if (m_device != nullptr && wParam != SIZE_MINIMIZED)
        {
            m_presentParameters.BackBufferWidth = LOWORD(lParam);
            m_presentParameters.BackBufferHeight = HIWORD(lParam);
            resetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_LBUTTONDOWN:
        m_position = MAKEPOINTS(lParam);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_MOUSEMOVE:

        if (wParam == MK_LBUTTON)
        {
            const auto points = MAKEPOINTS(lParam);
            auto rect = RECT{};

            GetWindowRect(m_window, &rect);

            rect.left += points.x - m_position.x;
            rect.top += points.y - m_position.y;

            if (m_position.x >= 0 && m_position.x < WIDTH
                && m_position.y >= 0 && m_position.y < HEIGHT)
            {
                SetWindowPos(m_window, HWND_TOPMOST,
                             rect.left,
                             rect.top,
                             0, 0,
                             SWP_SHOWWINDOW | SWP_NOZORDER);
            }
        }

    default:;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

void Gui::createWindow(const char* windowName, const char* className) noexcept
{
    m_windowClass.cbSize = sizeof(WNDCLASSEXA);
    m_windowClass.style = CS_CLASSDC;
    m_windowClass.lpfnWndProc = &windowProcess;
    m_windowClass.cbClsExtra = 0;
    m_windowClass.cbWndExtra = 0;
    m_windowClass.hInstance = GetModuleHandleA(0);
    m_windowClass.hIcon = 0;
    m_windowClass.hCursor = 0;
    m_windowClass.hbrBackground = 0;
    m_windowClass.lpszMenuName = 0;
    m_windowClass.lpszClassName = className;
    m_windowClass.hIconSm = 0;

    RegisterClassExA(&m_windowClass);

    setWindow(CreateWindowA(className, windowName, WS_POPUP,
        100, 100, WIDTH, HEIGHT, 0, 0, m_windowClass.hInstance, 0));

    ShowWindow(getWindow(), SW_SHOWDEFAULT);
    UpdateWindow(getWindow());
}

void Gui::destroyWindow() noexcept
{
    DestroyWindow(getWindow());
    UnregisterClass(m_windowClass.lpszClassName, m_windowClass.hInstance);
}

bool Gui::createDevice() noexcept
{
    setDirect3D9(Direct3DCreate9(D3D_SDK_VERSION));

    if (!getDirect3D9())
    {
        return false;
    }

    ZeroMemory(&m_presentParameters, sizeof(m_presentParameters));

    m_presentParameters.Windowed = TRUE;
    m_presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    m_presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
    m_presentParameters.EnableAutoDepthStencil = TRUE;
    m_presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
    m_presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    HRESULT hr = getDirect3D9()->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_window,
    D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_presentParameters, &m_device);

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

void Gui::resetDevice() noexcept
{
    ImGui_ImplDX9_InvalidateDeviceObjects();

    const auto result = getDevice()->Reset(&m_presentParameters);

    if (result == D3DERR_INVALIDCALL)
    {
        IM_ASSERT(0);
    }

    ImGui_ImplDX9_CreateDeviceObjects();
}

void Gui::destroyDevice() noexcept
{
    if (getDevice())
    {
        getDevice()->Release();
        setDevice(nullptr);
    }

    if (getDirect3D9())
    {
        getDirect3D9()->Release();
        setDirect3D9(nullptr);
    }
}

void Gui::createImGui() noexcept
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ::ImGui::GetIO();

    io.IniFilename = nullptr;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(m_window);
    ImGui_ImplDX9_Init(getDevice());
}

void Gui::destroyImGui() noexcept
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void Gui::beginRender() noexcept
{
    MSG message;
    while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    //Start the Dear ImgGui frame
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void Gui::endRender() noexcept
{
    ImGui::EndFrame();
    getDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
    getDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    getDevice()->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    getDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);
    if (getDevice()->BeginScene() >= 0)
    {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        getDevice()->EndScene();
    }

    HRESULT result = getDevice()->Present(nullptr, nullptr, nullptr, nullptr);

    // Handle loss of D3D9 device
    if (result == D3DERR_DEVICELOST && getDevice()->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
    {
        resetDevice();
    }
}

void Gui::render() noexcept
{
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize({WIDTH, HEIGHT});
    ImGui::Begin(
        "Teste",
        &m_exit,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove
    );

    ImGui::End();
}
