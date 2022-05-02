#pragma once
#include <unordered_map>

#include "pch.hpp"

namespace engine
{
    using WindowClass = WNDCLASSEXW;
    class Window;

    using WindowCallback =
        std::function<LRESULT(Window &, HWND, UINT, WPARAM, LPARAM)>;

    using MainLoopCallback = std::function<void(Window &)>;

    // Simple WINAPI window wrapper
    class Window
    {
    public:
        Window(WindowClass const &window_class_template, DWORD extended_style,
               std::wstring const &class_name, std::wstring const &window_name,
               DWORD style, math::ivec2 position, math::ivec2 size,
               HWND parent_window, HMENU menu, HINSTANCE instance, LPVOID lp_param);
        // remove copy and move semantics because the callback system is bound to the
        // address of the window
        Window(Window &&Window) = delete;
        Window &operator=(Window &&Window) = delete;
        Window(Window const &Window) = delete;
        Window &operator=(Window const &Window) = delete;

        virtual ~Window() { DestroyWindow(handle_); }

        // Windows event callback
        virtual void SetCallback(UINT message, WindowCallback const &function)
        {
            callbacks_.try_emplace(message, function);
        }

        virtual void SetMainLoopCallback(MainLoopCallback const &callback)
        {
            main_loop_callback_ = callback;
        }

        [[nodiscard]] constexpr HWND handle() const noexcept { return handle_; }
        [[nodiscard]] inline math::ivec2 size() const noexcept { return size_; }
        [[nodiscard]] inline math::ivec2 position() const noexcept
        {
            return position_;
        }

        void StartMainLoop();

    protected:
        virtual void OnSizeChanged() {}

    private:
        LRESULT CALLBACK WindowProcCallback(HWND handle, UINT message, WPARAM w_param,
                                            LPARAM l_param);

        static LRESULT CALLBACK StaticWindowProc(HWND handle, UINT message,
                                                 WPARAM w_param, LPARAM l_param);

        HWND handle_;
        std::unordered_map<UINT, WindowCallback> callbacks_;
        MainLoopCallback main_loop_callback_;
        math::ivec2 position_;
        math::ivec2 size_;
    };
}; // namespace engine