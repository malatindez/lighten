#pragma once
#include <unordered_map>

#include "pch.hpp"

#include "core/events/mouse-events.hpp"
#include "events.hpp"
#include "math/vec2.hpp"

namespace engine
{
    class Window;

    using WindowCallback =
        std::function<LRESULT(Window &, HWND, UINT, WPARAM, LPARAM)>;

    // Simple WINAPI window wrapper
    class Window
    {
    public:
        Window(WNDCLASSEXW const &window_class_template, DWORD extended_style,
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

        void SetEventCallback(EventCallbackFn const &callback) noexcept
        {
            event_callback_ = callback;
        }

        [[nodiscard]] constexpr HWND handle() const noexcept { return handle_; }
        [[nodiscard]] constexpr math::ivec2 const &size() const noexcept { return size_; }
        [[nodiscard]] constexpr math::ivec2 const &position() const noexcept { return position_; }
        [[nodiscard]] constexpr bool running() const noexcept { return running_; }

        virtual bool PeekOSMessages();

    protected:
        virtual void OnSizeChanged() {}

    private:
        LRESULT CALLBACK WindowProcCallback(HWND handle, UINT message, WPARAM w_param, LPARAM l_param);
        static LRESULT CALLBACK StaticWindowProc(HWND handle, UINT message, WPARAM w_param, LPARAM l_param);

        EventCallbackFn event_callback_;

        bool running_ = true;
        HWND handle_;
        math::ivec2 position_;
        math::ivec2 size_;
    };
}; // namespace engine