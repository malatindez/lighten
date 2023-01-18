#pragma once
#include "include/engine-pch.hpp"
#include "include/win.hpp"

#include "core/window.hpp"
#include "utils/utils.hpp"

namespace engine::platform::windows
{
    // Simple WINAPI window wrapper
    class Window : public core::Window
    {
    public:
        using WindowCallback = std::function<LRESULT(Window &, HWND, UINT, WPARAM, LPARAM)>;
        struct WindowError : std::runtime_error
        {
            using std::runtime_error::runtime_error;
        };

        Window(core::Window::Props const &props = core::Window::Props(core::config()["Window"]["title"].to_string(),
                                                                      core::math::ivec2{ core::config()["Window"]["width"].to_int(), core::config()["Window"]["height"].to_int() },
                                                                      core::math::ivec2{ core::config()["Window"]["posx"].to_int(), core::config()["Window"]["posy"].to_int() }));

        virtual ~Window() { DestroyWindow(handle_); }

        [[nodiscard]] constexpr HWND handle() const noexcept { return handle_; }
        [[nodiscard]] constexpr bool running() const noexcept { return alive_; }

        [[nodiscard]] void *native() override { return handle(); };
        // TODO(malatindez) make this method private and handle events using GetMessage
        // save main-thread dependant events in event pool and pass them if Window->OnUpdate is called
        void OnUpdate() override;

    private:
        // remove copy and move semantics because the callback system is bound to the
        // address of the window
        Window(Window &&Window) = delete;
        Window &operator=(Window &&Window) = delete;
        Window(Window const &Window) = delete;
        Window &operator=(Window const &Window) = delete;

        LRESULT CALLBACK WindowProcCallback(HWND handle, UINT message, WPARAM w_param,
                                            LPARAM l_param);
        static LRESULT CALLBACK StaticWindowProc(HWND handle, UINT message,
                                                 WPARAM w_param, LPARAM l_param);

        HWND handle_;
    };
}; // namespace engine::core