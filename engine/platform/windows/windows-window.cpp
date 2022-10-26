#include "windows-window.hpp"
#include "include/imgui.hpp"
#include <ImGuizmo.h>
#include "utils/win-utils.hpp"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace engine::platform::windows
{
    using namespace core::events;
    Window::Window(Props const &props) : core::Window(props)
    {
        WNDCLASSEXW wc;
        // clear out the window class for use
        ZeroMemory(&wc, sizeof(WNDCLASSEX));

        // fill in the struct with the needed information
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hCursor = LoadCursor(nullptr, IDC_CROSS);
        wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
        wc.lpszClassName = L"WindowClass1";
        wc.lpfnWndProc = Window::StaticWindowProc;
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.cbWndExtra = sizeof(void *);

        RegisterClassExW(&wc);

        handle_ = CreateWindowExW(NULL, wc.lpszClassName, utils::string_to_wstring(title_).c_str(),
                                  WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                  position_.x, position_.y, size_.x, size_.y,
                                  nullptr, nullptr, GetModuleHandle(NULL), nullptr);

        // save window handle in the window class
        SetWindowLongPtrW(handle_, 0, reinterpret_cast<LONG_PTR>(this));

        if (!handle_)
        {
            MessageBoxW(nullptr, L"CreateWindowEx failed", nullptr, 0);
            UnregisterClassW(wc.lpszClassName, GetModuleHandle(NULL));
            throw GetLastError();
        }

        // display the window on the screen
        ShowWindow(handle(), 10);
    }
    void Window::OnUpdate()
    {
        if (!alive_)
        {
            return;
        }
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) // doesnt work
            {
                alive_ = false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (!IsWindow(handle())) // temporary workaround
        {
            alive_ = false;
            core::events::WindowCloseEvent wce{ handle() };
            event_callback_(wce);
        }
        return;
    }
    LRESULT CALLBACK Window::WindowProcCallback(HWND handle, UINT message,
                                                WPARAM w_param, LPARAM l_param)
    {
        if (ImGui_ImplWin32_WndProcHandler(handle, message, w_param, l_param))
            return true;
        if (message == WM_KEYDOWN)
        {
            KeyPressedEvent event{ uint32_t(w_param), LOWORD(l_param) };
            event_callback_(event);
        }
        else if (message == WM_KEYUP)
        {
            KeyReleasedEvent event{ uint32_t(w_param) };
            event_callback_(event);
        }
        else if (message == WM_MOUSEMOVE)
        {
            MouseMovedEvent event{ core::math::ivec2{(int16_t)(LOWORD(l_param)),(int16_t)(HIWORD(l_param))} };
            event_callback_(event);
        }
        else if (message == WM_LBUTTONDOWN)
        {
            MouseButtonPressedEvent event{ 0, core::math::ivec2{LOWORD(l_param), HIWORD(l_param)} };
            event_callback_(event);
        }
        else if (message == WM_LBUTTONUP)
        {
            MouseButtonReleasedEvent event{ 0, core::math::ivec2{LOWORD(l_param), HIWORD(l_param)} };
            event_callback_(event);
        }
        else if (message == WM_RBUTTONDOWN)
        {
            MouseButtonPressedEvent event{ 1, core::math::ivec2{LOWORD(l_param), HIWORD(l_param)} };
            event_callback_(event);
        }
        else if (message == WM_RBUTTONUP)
        {
            MouseButtonReleasedEvent event{ 1, core::math::ivec2{LOWORD(l_param), HIWORD(l_param)} };
            event_callback_(event);
        }
        else if (message == WM_MOUSEWHEEL)
        {
            MouseScrollEvent event{ GET_WHEEL_DELTA_WPARAM(w_param), core::math::ivec2{LOWORD(l_param), HIWORD(l_param)} };
            event_callback_(event);
        }
        else if (message == WM_SIZE) // update the window size if it has changed
        {
            if (l_param != 0)
            {
                WindowResizeEvent event{ LOWORD(l_param), HIWORD(l_param) };
                size_ = core::math::ivec2{ LOWORD(l_param), HIWORD(l_param) };
                event_callback_(event);
            }
        }
        else if (message == WM_EXITSIZEMOVE || message == WM_PAINT)
        {
            WindowResizeEvent event{ size_.x, size_.y };
            event_callback_(event);
        }
        else if (message == WM_WINDOWPOSCHANGED) // update window position if it has changed
        {
            auto window_pos = reinterpret_cast<LPWINDOWPOS>(l_param);
            position_ = core::math::ivec2{ window_pos->x, window_pos->y };
        }
        else if (message == WM_DESTROY)
        {
            WindowCloseEvent event{ handle_ };
            event_callback_(event);
        }
        return DefWindowProcW(handle, message, w_param, l_param);
    }

    LRESULT CALLBACK Window::StaticWindowProc(HWND handle, UINT message,
                                              WPARAM w_param, LPARAM l_param)
    {
        if (auto ptr = reinterpret_cast<Window *>(GetWindowLongPtrW(handle, 0));
            ptr != nullptr)
        {
            return ptr->WindowProcCallback(handle, message, w_param, l_param);
        }
        return DefWindowProcW(handle, message, w_param, l_param);
    }
} // namespace engine::platform::windows