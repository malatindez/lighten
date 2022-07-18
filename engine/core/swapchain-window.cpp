#include "swapchain-window.hpp"
#include "direct3d/globals.hpp"
#include "window.hpp"
#include "core/debug_utils.hpp"
#include <unordered_map>

namespace engine::core
{

    SwapchainWindow::SwapchainWindow(WNDCLASSEXW const &window_class,
                               DWORD extended_style, std::wstring const &class_name,
                               std::wstring const &window_name, DWORD style,
                               core::math::ivec2 position, core::math::ivec2 size,
                               HWND parent_window, HMENU menu, HINSTANCE instance,
                               LPVOID lp_param)
        : Window(window_class, extended_style, class_name, window_name, style,
                 position, size, parent_window, menu, instance, lp_param)
    {
        initialize();
    }
    
    void SwapchainWindow::OnSizeChangeEnd()
    {
        if (frame_buffer_.valid())
        {
            frame_buffer_.reset();
            frame_buffer_view_.reset();
            depth_buffer_.reset();
            depth_buffer_view_.reset();
            swapchain_->ResizeBuffers(0, window_size().x, window_size().y, DXGI_FORMAT_UNKNOWN, 0);
        }
        initializeFramebuffer();
        initializeDepthbuffer();

    }
    direct3d::SwapChain1 initializeSwapchain(HWND hWnd)
    {
        DXGI_SWAP_CHAIN_DESC1 desc;
        ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC1));
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        desc.Stereo = FALSE;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.BufferCount = 2;
        desc.Scaling = DXGI_SCALING_STRETCH;
        desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        desc.Flags = 0;

        IDXGISwapChain1* swapchain;
        if (FAILED(direct3d::factory5->CreateSwapChainForHwnd(direct3d::device5, hWnd, &desc, nullptr, nullptr, &swapchain)))
        {
            throw SwapchainWindow::SwapchainWindowError(core::debug_utils::CurrentSourceLocation() + "Failed to initialize swapchain for hwnd");
        }
        return direct3d::SwapChain1{ swapchain };
    }

    void SwapchainWindow::initializeFramebuffer()
    {
        ID3D11Texture2D* frame_buffer = nullptr;

        if (FAILED(swapchain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&frame_buffer))))
        {
            throw SwapchainWindow::SwapchainWindowError(core::debug_utils::CurrentSourceLocation() + "Failed to initialize framebuffer");
        }

        ID3D11RenderTargetView* frame_buffer_view = nullptr;

        if (FAILED(direct3d::device->CreateRenderTargetView(frame_buffer, nullptr, &frame_buffer_view)))
        {
            throw SwapchainWindow::SwapchainWindowError(core::debug_utils::CurrentSourceLocation() + "Failed to initialize framebuffer");
        }
        frame_buffer_ = frame_buffer;
        frame_buffer_view_ = frame_buffer_view;
    }

    void SwapchainWindow::initializeDepthbuffer()
    {
        frame_buffer_->GetDesc(&depth_buffer_desc_); // base on frame_buffer properties

        depth_buffer_desc_.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depth_buffer_desc_.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        ID3D11Texture2D* depth_buffer;


        if (FAILED(direct3d::device->CreateTexture2D(&depth_buffer_desc_, nullptr, &depth_buffer)))
        {
            throw SwapchainWindow::SwapchainWindowError(core::debug_utils::CurrentSourceLocation() + "Failed to initialize depthbuffer");
        }

        ID3D11DepthStencilView* depth_buffer_view;

        if(FAILED(direct3d::device->CreateDepthStencilView(depth_buffer, nullptr, &depth_buffer_view)))
        {
            throw SwapchainWindow::SwapchainWindowError(core::debug_utils::CurrentSourceLocation() + "Failed to initialize depthbuffer");
        }
        depth_buffer_ = depth_buffer;
        depth_buffer_view_ = depth_buffer_view;
    }


    void SwapchainWindow::initialize()
    {
        swapchain_ = initializeSwapchain(handle());
        initializeFramebuffer();
        initializeDepthbuffer();

    }

} // namespace engine::core