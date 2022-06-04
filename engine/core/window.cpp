#include "core/window.hpp"
namespace engine::core
{
  using namespace events;
  Window::Window(WNDCLASSEXW const &window_class_template, DWORD extended_style,
                 std::wstring const &class_name, std::wstring const &window_name,
                 DWORD style, core::math::ivec2 position, core::math::ivec2 size,
                 HWND parent_window, HMENU menu, HINSTANCE instance,
                 LPVOID lp_param)
      : position_(position), window_size_(size)
  {
    // Create custom window_class for this window
    WNDCLASSEXW window_class{window_class_template};
    window_class.lpszClassName = class_name.c_str();
    window_class.lpfnWndProc = Window::StaticWindowProc;
    window_class.cbSize = sizeof(WNDCLASSEXW);
    window_class.cbWndExtra = sizeof(void *);

    RegisterClassExW(&window_class);

    handle_ =
        CreateWindowExW(extended_style, class_name.c_str(), window_name.c_str(),
                        style, position.x, position.y, size.x, size.y,
                        parent_window, menu, instance, lp_param);

    // save window handle in the window class
    SetWindowLongPtrW(handle_, 0, reinterpret_cast<LONG_PTR>(this));

    if (!handle_)
    {
      MessageBoxW(nullptr, L"CreateWindowEx failed", nullptr, 0);
      UnregisterClassW(class_name.c_str(), instance);
      throw GetLastError();
    }
  }

  bool Window::PeekOSMessages()
  {
    if (!running_)
    {
      return false;
    }
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
      if (msg.message == WM_QUIT)
      {
        running_ = false;
      }
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    return true;
  }

  LRESULT CALLBACK Window::WindowProcCallback(HWND handle, UINT message,
                                              WPARAM w_param, LPARAM l_param)
  {
    // update the window size if it has changed
    if (message == WM_SIZE)
    {
      window_size_ = core::math::ivec2{LOWORD(l_param), HIWORD(l_param)};
      OnSizeChanged();
    } // update window position if it has changed
    else if (message == WM_WINDOWPOSCHANGED)
    {
      auto window_pos = reinterpret_cast<LPWINDOWPOS>(l_param);
      position_ = core::math::ivec2{window_pos->x, window_pos->y};
    }

    if (message == WM_KEYDOWN)
    {
      KeyPressedEvent event{uint32_t(w_param), LOWORD(l_param)};
      event_callback_(event);
    }
    else if (message == WM_KEYUP)
    {
      KeyReleasedEvent event{uint32_t(w_param)};
      event_callback_(event);
    }
    else if (message == WM_MOUSEMOVE)
    {
      MouseMovedEvent event{core::math::ivec2{LOWORD(l_param), HIWORD(l_param)}};
      event_callback_(event);
    }
    else if (message == WM_LBUTTONDOWN)
    {
      MouseButtonPressedEvent event{
          0, core::math::ivec2{LOWORD(l_param), HIWORD(l_param)}};
      event_callback_(event);
    }
    else if (message == WM_LBUTTONUP)
    {
      MouseButtonReleasedEvent event{
          0, core::math::ivec2{LOWORD(l_param), HIWORD(l_param)}};
      event_callback_(event);
    }
    else if (message == WM_RBUTTONDOWN)
    {
      MouseButtonPressedEvent event{
          1, core::math::ivec2{LOWORD(l_param), HIWORD(l_param)}};
      event_callback_(event);
    }
    else if (message == WM_RBUTTONUP)
    {
      MouseButtonReleasedEvent event{
          1, core::math::ivec2{LOWORD(l_param), HIWORD(l_param)}};
      event_callback_(event);
    }
    else if (message == WM_DESTROY)
    {
      WindowCloseEvent event{handle_};
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

} // namespace engine::core