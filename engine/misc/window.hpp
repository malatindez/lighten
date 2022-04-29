#pragma once
#include <unordered_map>

#include "pch.hpp"

namespace engine {

using WindowClass = WNDCLASSEXW;
class Window;

using WindowCallback =
    std::function<LRESULT(Window&, HWND, UINT, WPARAM, LPARAM)>;

using MainLoopCallback = std::function<void(Window&)>;

// Simple WINAPI window wrapper
class Window {
 public:
  Window(WindowClass const& window_class_template, DWORD extended_style,
         std::wstring const& class_name, std::wstring const& window_name,
         DWORD style, math::ivec2 position, math::ivec2 size,
         HWND parent_window, HMENU menu, HINSTANCE instance, LPVOID lp_param)
      : position_(position), size_(size) {
    // Create custom window_class for this window
    WindowClass window_class{window_class_template};
    window_class.lpszClassName = class_name.c_str();
    window_class.lpfnWndProc = Window::StaticWindowProc;
    window_class.cbSize = sizeof(WindowClass);
    window_class.cbWndExtra = sizeof(void*);

    RegisterClassExW(&window_class);
    handle_ =
        CreateWindowExW(extended_style, class_name.c_str(), window_name.c_str(),
                        style, position.x(), position.y(), size.x(), size.y(),
                        parent_window, menu, instance, lp_param);
    // save window handle in the window class
    SetWindowLongPtrW(handle_, 0, reinterpret_cast<LONG_PTR>(this));

    if (!handle_) {
      MessageBoxW(nullptr, L"CreateWindowEx failed", nullptr, 0);
      UnregisterClassW(class_name.c_str(), instance);
      throw GetLastError();
    }
  }
  // remove copy and move semantics because the callback system is bound to the address of the window
  Window(Window&& Window) = delete;
  Window& operator=(Window&& Window) = delete;
  Window(Window const& Window) = delete;
  Window& operator=(Window const& Window) = delete;

  virtual ~Window() { DestroyWindow(handle_); }

  // Windows event callback
  virtual void SetCallback(UINT message, WindowCallback const& function) {
    callbacks_.try_emplace(message, function);
  }

  // Main loop callback
  virtual void SetMainLoopCallback(MainLoopCallback const& callback) {
    main_loop_callback_ = callback;
  }

  [[nodiscard]] constexpr HWND handle() const noexcept { return handle_; }
  [[nodiscard]] inline math::ivec2 size() const noexcept { return size_; }
  [[nodiscard]] inline math::ivec2 position() const noexcept {
    return position_;
  }

  void StartMainLoop() {
    bool running = true;
    while (running) {
      MSG msg;
      while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
          running = false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
      if (main_loop_callback_ != nullptr) {
        main_loop_callback_(std::ref(*this));
      }
    }
  }

 protected:
  virtual void OnSizeChanged() {}

 private:
  LRESULT CALLBACK WindowProcCallback(HWND handle, UINT message, WPARAM w_param,
                                      LPARAM l_param) {
    // update the window size if it has changed
    if (message == WM_SIZE) {
      size_ = math::ivec2{LOWORD(l_param), HIWORD(l_param)};
      OnSizeChanged();
    }  // update window position if it has changed
    else if (message == WM_WINDOWPOSCHANGED) {
      auto window_pos = reinterpret_cast<LPWINDOWPOS>(l_param);
      position_ = math::ivec2{window_pos->x, window_pos->y};
    }
    // call callback if it exists
    if (auto it = callbacks_.find(message); it != callbacks_.end()) {
      return it->second(std::ref(*this), handle, message, w_param, l_param);
    }
    // handle any messages that callbacks didnt
    return DefWindowProcW(handle, message, w_param, l_param);
  }

  static LRESULT CALLBACK StaticWindowProc(HWND handle, UINT message,
                                           WPARAM w_param, LPARAM l_param) {
    if (auto ptr = reinterpret_cast<Window*>(GetWindowLongPtrW(handle, 0));
        ptr != nullptr) {
      return ptr->WindowProcCallback(handle, message, w_param, l_param);
    }
    return DefWindowProcW(handle, message, w_param, l_param);
  }

  HWND handle_;
  std::unordered_map<UINT, WindowCallback> callbacks_;
  MainLoopCallback main_loop_callback_;
  math::ivec2 position_;
  math::ivec2 size_;
};
};  // namespace engine