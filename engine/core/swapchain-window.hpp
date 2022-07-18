#pragma once
#include <unordered_map>
#include "window.hpp"
#include "direct3d/common.hpp"

namespace engine::core
{
    class SwapchainWindow final : public Window
    {
    public:
        struct SwapchainWindowError : std::runtime_error
        {
            using std::runtime_error::runtime_error;
        };

        SwapchainWindow(WNDCLASSEXW const &window_class, DWORD extended_style,
                     std::wstring const &class_name, std::wstring const &window_name,
                     DWORD style, math::ivec2 position, math::ivec2 size,
                     HWND parent_window, HMENU menu, HINSTANCE instance,
                     LPVOID lp_param);
        SwapchainWindow(SwapchainWindow &&) = delete;
        SwapchainWindow &operator=(SwapchainWindow &&) = delete;
        SwapchainWindow(SwapchainWindow const &) = delete;
        SwapchainWindow &operator=(SwapchainWindow const &) = delete;
        
        [[nodiscard]] IDXGISwapChain1 *swapchain() noexcept { return swapchain_.ptr(); }
        [[nodiscard]] ID3D11Texture2D *frame_buffer() noexcept {return frame_buffer_.ptr(); }
        [[nodiscard]] ID3D11RenderTargetView *frame_buffer_view() noexcept {return frame_buffer_view_.ptr(); }
        [[nodiscard]] ID3D11Texture2D *depth_buffer() noexcept {return depth_buffer_.ptr(); }
        [[nodiscard]] ID3D11DepthStencilView* depth_buffer_view() noexcept { return depth_buffer_view_.ptr(); }
        [[nodiscard]] D3D11_TEXTURE2D_DESC depth_buffer_desc() const noexcept { return depth_buffer_desc_; }

    private:
        void initialize();
        void initializeFramebuffer();
        void initializeDepthbuffer();

        void OnSizeChangeEnd() override;

        direct3d::SwapChain1 swapchain_;
        direct3d::Texture2D frame_buffer_;
        direct3d::RenderTargetView frame_buffer_view_;
        direct3d::Texture2D depth_buffer_;
        direct3d::DepthStencilView depth_buffer_view_;
		D3D11_TEXTURE2D_DESC depth_buffer_desc_;
    };
} // namespace engine::core