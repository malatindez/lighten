#include "render-target.hpp"
#include "direct3d11/d3d-debug.hpp"
#include "direct3d11/api.hpp"

namespace engine::direct3d
{
    RenderTargetBase::RenderTargetBase(DXGI_FORMAT format) noexcept :
        format_(format),
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

    void RenderTarget::SizeResources(core::math::ivec2 const &size)
    {
        if (size == size_)
            return;

        if (size.x < 0 || size.y < 0)
        {
            throw std::out_of_range("Invalid width/height");
        }

        size_ = core::math::ivec2{ 0 };

        // Create a render target
        CD3D11_TEXTURE2D_DESC renderTargetDesc(
            format_,
            static_cast<UINT>(size.x),
            static_cast<UINT>(size.y),
            1, // The render target view has only one texture.
            1, // Use a single mipmap level.
            D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
            D3D11_USAGE_DEFAULT,
            0,
            1
        );
        Assert(api().device->CreateTexture2D(
            &renderTargetDesc,
            nullptr,
            &render_target_.reset()
        ));

        CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, format_);

        Assert(api().device->CreateRenderTargetView(
            render_target_.ptr(),
            &renderTargetViewDesc,
            &render_target_view_.reset()
        ));

        // Create SRV.
        CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE2D, format_);

        Assert(api().device->CreateShaderResourceView(
            render_target_.ptr(),
            &shaderResourceViewDesc,
            &shader_resource_view_.reset()
        ));

        size_ = size;
    }

    void RenderTarget::deinit() noexcept
    {
        render_target_view_.reset();
        shader_resource_view_.reset();
        render_target_.reset();
        size_ = core::math::ivec2{ 0 };
        initialized = false;
    }

    void SwapchainRenderTarget::init(SwapChain1 const &swapchain)
    {
        if (swapchain_.valid())
        {
            deinit();
        }
        swapchain_ = swapchain;
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