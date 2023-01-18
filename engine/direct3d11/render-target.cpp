#include "render-target.hpp"
#include "direct3d11/api.hpp"
#include "direct3d11/d3d-debug.hpp"

namespace engine::direct3d
{
    RenderTargetBase::RenderTargetBase(DXGI_FORMAT format) noexcept : format_(format),
        size_(0, 0)
    {
    }

    void RenderTarget::init()
    {
        if (initialized)
        {
            deinit();
        }
        UINT formatSupport = 0;
        if (FAILED(api().device->CheckFormatSupport(format_, &formatSupport)))
        {
            throw std::runtime_error("CheckFormatSupport");
        }

        constexpr UINT32 required = D3D11_FORMAT_SUPPORT_TEXTURE2D | D3D11_FORMAT_SUPPORT_RENDER_TARGET;
        if ((formatSupport & required) != required)
        {
            spdlog::error("RenderTarget: Device does not support the requested format (%u)!", format_);
            throw std::runtime_error("RenderTarget");
        }
        initialized = true;
    }
    void RenderTarget::ForceSizeResources(core::math::ivec2 const &size)
    {
        if (size.x < 0 || size.y < 0)
        {
            throw std::out_of_range("Invalid width/height");
        }

        size_ = core::math::ivec2{ 0 };
        render_target_description_.Format = format_;
        render_target_description_.Width = static_cast<UINT>(size.x);
        render_target_description_.Height = static_cast<UINT>(size.y);
        render_target_description_.MipLevels = 1;
        render_target_description_.ArraySize = 1;
        render_target_description_.SampleDesc.Count = sample_count;
        render_target_description_.SampleDesc.Quality = (UINT)D3D11_STANDARD_MULTISAMPLE_PATTERN;
        render_target_description_.Usage = D3D11_USAGE_DEFAULT;
        render_target_description_.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        render_target_description_.CPUAccessFlags = 0;
        render_target_description_.MiscFlags = 0;
        Assert(api().device->CreateTexture2D(
            &render_target_description_,
            nullptr,
            &render_target_.reset()
        ));

        D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc;
        render_target_view_desc.Format = render_target_description_.Format;
        render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
        render_target_view_desc.Texture2D.MipSlice = 0;

        Assert(api().device->CreateRenderTargetView(
            render_target_.ptr(),
            &render_target_view_desc,
            &render_target_view_.reset()
        ));

        // Create SRV.
        D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc;
        shader_resource_view_desc.Format = render_target_description_.Format;
        shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
        shader_resource_view_desc.Texture2D.MostDetailedMip = 0;
        shader_resource_view_desc.Texture2D.MipLevels = 1;

        Assert(api().device->CreateShaderResourceView(
            render_target_.ptr(),
            &shader_resource_view_desc,
            &shader_resource_view_.reset()
        ));

        size_ = size;
    }

    void RenderTarget::SizeResources(core::math::ivec2 const &size)
    {
        if (size == size_)
            return;
        ForceSizeResources(size);
    }

    void RenderTarget::deinit() noexcept
    {
        render_target_view_.reset();
        shader_resource_view_.reset();
        render_target_.reset();
        size_ = core::math::ivec2{ 0 };
        initialized = false;
    }

    void SwapchainRenderTarget::init(HWND hWnd, core::math::ivec2 const &window_size)
    {
        if (swapchain_.valid())
        {
            deinit();
        }
        DXGI_SWAP_CHAIN_DESC1 desc;
        ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC1));
        desc.Width = window_size.x;
        desc.Height = window_size.y;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.Stereo = FALSE;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.BufferCount = 2;
        desc.Scaling = DXGI_SCALING_NONE;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        desc.Flags = 0;

        IDXGISwapChain1 *swapchain = nullptr;
        SetLastError(0);
        AlwaysAssert(api().factory5->CreateSwapChainForHwnd(api().device, hWnd, &desc, nullptr, nullptr, &swapchain),
                     "Failed to create the swapchain");
        swapchain_ = SwapChain1{ swapchain };
        SizeResources(core::math::ivec2{ 0 });
        initialized = true;
    }
    void SwapchainRenderTarget::deinit() noexcept
    {
        render_target_.reset();
        render_target_view_.reset();
        shader_resource_view_.reset();
        swapchain_.reset();
        initialized = false;
    }
    void SwapchainRenderTarget::SizeResources(core::math::ivec2 const &size)
    {
        if (!swapchain_.valid() || size == size_)
        {
            return;
        }
        direct3d::api().devcon4->OMSetRenderTargets(0, nullptr, nullptr);
        render_target_.reset();
        render_target_view_.reset();
        shader_resource_view_.reset();
        swapchain_->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
        AlwaysAssert(swapchain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&render_target_.reset())),
                     "Failed to get frame buffer");
        render_target_->GetDesc(&render_target_description_);
        format_ = render_target_description_.Format;
        size_.x = size.x;
        size_.y = size.y;
        AlwaysAssert(api().device->CreateRenderTargetView(render_target_.ptr(), nullptr, &render_target_view_.reset()),
                     "Failed to initialize framebuffer");
    }
}