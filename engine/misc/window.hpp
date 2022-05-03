#pragma once
#include <unordered_map>

#include "interfaces/Updatable.hpp"
#include "pch.hpp"

namespace engine
{
    using WindowClass = WNDCLASSEXW;
    class Window;

    using WindowCallback =
        std::function<LRESULT(Window &, HWND, UINT, WPARAM, LPARAM)>;

    using MainLoopCallback = std::function<void(Window &)>;

    // Simple WINAPI window wrapper
    class Window : public interfaces::Updatable
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

        [[nodiscard]] constexpr HWND handle() const noexcept { return handle_; }
        [[nodiscard]] constexpr math::ivec2 size() const noexcept { return size_; }
        [[nodiscard]] constexpr math::ivec2 position() const noexcept { return position_; }
        [[nodiscard]] constexpr bool running() const noexcept { return running_; }
        bool Update() override;

    protected:
        virtual void OnSizeChanged() {}

    private:
        LRESULT CALLBACK WindowProcCallback(HWND handle, UINT message, WPARAM w_param,
                                            LPARAM l_param);

        static LRESULT CALLBACK StaticWindowProc(HWND handle, UINT message,
                                                 WPARAM w_param, LPARAM l_param);
        bool running_ = true;
        HWND handle_;
        std::unordered_map<UINT, WindowCallback> callbacks_;
        MainLoopCallback main_loop_callback_;
        math::ivec2 position_;
        math::ivec2 size_;
    };
}; // namespace engine