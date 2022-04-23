#include "Gui.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx9.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace myApp::gui;

constexpr int WIDTH = 500;
constexpr int HEIGHT = 300;
inline HWND                  window = nullptr;
inline WNDCLASSEXA           windowClass = {};
inline POINTS                position = {};
inline PDIRECT3D9            d3d = nullptr;
inline LPDIRECT3DDEVICE9     device = nullptr;
inline D3DPRESENT_PARAMETERS presentParameters = {};

namespace
{
    void resetDevice() noexcept
    {
        ImGui_ImplDX9_InvalidateDeviceObjects();

        const auto result = device->Reset(&presentParameters);

        if (result == D3DERR_INVALIDCALL)
        {
            IM_ASSERT(0);
        }

        ImGui_ImplDX9_CreateDeviceObjects();
    }

} //namespace

Gui::Gui(const char* windowName, const char* className):
    m_exit(false)
{}

void Gui::createWindow(const char* windowName, const char* className) noexcept
{
    windowClass.cbSize = sizeof(WNDCLASSEXA);
    windowClass.style = CS_CLASSDC;
    windowClass.lpfnWndProc = windowProcess;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = GetModuleHandleA(nullptr);
    windowClass.hIcon = nullptr;
    windowClass.hCursor = nullptr;
    windowClass.hbrBackground = nullptr;
    windowClass.lpszMenuName = nullptr;
    windowClass.lpszClassName = className;
    windowClass.hIconSm = nullptr;

    RegisterClassExA(&windowClass);

    window = CreateWindowA(className, windowName, WS_POPUP,
        100, 100, WIDTH, HEIGHT, 0, 0, windowClass.hInstance, 0);

    ShowWindow(window, SW_SHOWDEFAULT);
    UpdateWindow(window);
}

void Gui::destroyWindow() noexcept
{
    DestroyWindow(window);
    UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool Gui::createDevice() noexcept
{
    d3d = Direct3DCreate9(D3D_SDK_VERSION);

    if (!d3d)
    {
        return false;
    }

    ZeroMemory(&presentParameters, sizeof(presentParameters));

    presentParameters.Windowed = TRUE;
    presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
    presentParameters.EnableAutoDepthStencil = TRUE;
    presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
    presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    if (d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
        window,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &presentParameters, &device) < 0)
    {
        return false;
    }

    return true;
}

void Gui::destroyDevice() noexcept
{
    if (device)
    {
        device->Release();
        device = nullptr;
    }

    if (d3d)
    {
        d3d->Release();
        d3d = nullptr;
    }
}

void Gui::createImGui() noexcept
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ::ImGui::GetIO();

    io.IniFilename = nullptr;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX9_Init(device);
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
    device->SetRenderState(D3DRS_ZENABLE, FALSE);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);
    if (device->BeginScene() >= 0)
    {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        device->EndScene();
    }

    HRESULT result = device->Present(nullptr, nullptr, nullptr, nullptr);

    // Handle loss of D3D9 device
    if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
    {
        ::resetDevice();
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

LRESULT WINAPI myApp::gui::windowProcess(const HWND hWnd, const UINT msg, WPARAM wParam, const LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;

    switch (msg)
    {
    case WM_SIZE:
        if (device != nullptr && wParam != SIZE_MINIMIZED)
        {
            presentParameters.BackBufferWidth = LOWORD(lParam);
            presentParameters.BackBufferHeight = HIWORD(lParam);
            resetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_LBUTTONDOWN:
        position = MAKEPOINTS(lParam);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_MOUSEMOVE:

        if (wParam == MK_LBUTTON)
        {
            const auto points = MAKEPOINTS(lParam);
            auto rect = RECT{};

            GetWindowRect(window, &rect);

            rect.left += points.x - position.x;
            rect.top += points.y - position.y;

            if (position.x >= 0 && position.x < WIDTH
                && position.y >= 0 && position.y < HEIGHT)
            {
                SetWindowPos(window, HWND_TOPMOST,
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
