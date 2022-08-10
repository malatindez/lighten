#pragma once
#include <unordered_map>

#include "include/pch.hpp"
#include "include/win.hpp"

#include "direct3d11/direct3d11.hpp"

#include "core/events.hpp"
#include "core/math.hpp"
#include "core/window.hpp"

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

        Window(core::Window::Props const &props = core::Window::Props());

        virtual ~Window() { DestroyWindow(handle_); }

        [[nodiscard]] constexpr HWND handle() const noexcept { return handle_; }
        [[nodiscard]] constexpr bool running() const noexcept { return alive_; }

        [[nodiscard]] IDXGISwapChain1 *swapchain() noexcept { return swapchain_.ptr(); }
        [[nodiscard]] ID3D11Texture2D *frame_buffer() noexcept { return frame_buffer_.ptr(); }
        [[nodiscard]] ID3D11RenderTargetView *frame_buffer_view() noexcept { return frame_buffer_view_.ptr(); }
        [[nodiscard]] ID3D11Texture2D *depth_buffer() noexcept { return depth_buffer_.ptr(); }
        [[nodiscard]] ID3D11DepthStencilView *depth_buffer_view() noexcept { return depth_buffer_view_.ptr(); }
        [[nodiscard]] D3D11_TEXTURE2D_DESC depth_buffer_desc() const noexcept { return depth_buffer_desc_; }


        [[nodiscard]] void *native() override { return handle(); };
        // TODO(malatindez) make this method private and handle events using GetMessage
        // save main-thread dependant events in event pool and pass them if Window->OnUpdate is called
        bool OnUpdate();

    private:
        void initialize_d3d();
        void initializeFramebuffer();
        void initializeDepthbuffer();
        void OnSizeChangeEnd();

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

        direct3d::SwapChain1 swapchain_;
        direct3d::Texture2D frame_buffer_;
        direct3d::RenderTargetView frame_buffer_view_;
        direct3d::Texture2D depth_buffer_;
        direct3d::DepthStencilView depth_buffer_view_;
        D3D11_TEXTURE2D_DESC depth_buffer_desc_;
    };
}; // namespace engine::core