#pragma once

#include "direct3d11/d3d-common.hpp"
#include "core/math.hpp"

namespace engine::direct3d
{
    class RenderTargetBase
    {
    public:
        explicit RenderTargetBase(DXGI_FORMAT format) noexcept;

        RenderTargetBase(RenderTargetBase &&) = default;
        RenderTargetBase &operator= (RenderTargetBase &&) = default;

        RenderTargetBase(RenderTargetBase const &) = delete;
        RenderTargetBase &operator= (RenderTargetBase const &) = delete;

        virtual void SizeResources(core::math::ivec2 const &size) = 0;

        [[nodiscard]] inline core::math::ivec2 const &size() const noexcept { return size_; }
        ID3D11Texture2D const *const &render_target() const noexcept { return render_target_.ptr(); }
        ID3D11RenderTargetView const *const &render_target_view() const noexcept { return render_target_view_.ptr(); }
        ID3D11ShaderResourceView const *const &shader_resource_view() const noexcept { return shader_resource_view_.ptr(); }
        ID3D11Texture2D *const &render_target() noexcept { return render_target_.ptr(); }
        ID3D11RenderTargetView *const &render_target_view() noexcept { return render_target_view_.ptr(); }
        ID3D11ShaderResourceView *const &shader_resource_view() noexcept { return shader_resource_view_.ptr(); }
        D3D11_TEXTURE2D_DESC const &render_target_description() const noexcept { return render_target_description_; }

        DXGI_FORMAT GetFormat() const noexcept { return format_; }

    protected:
        bool initialized = false;
        Texture2D render_target_;
        RenderTargetView render_target_view_;
        ShaderResourceView shader_resource_view_;
        DXGI_FORMAT format_;
        D3D11_TEXTURE2D_DESC render_target_description_;
        core::math::ivec2 size_;
    };

    class RenderTarget : public RenderTargetBase
    {
    public:
        RenderTarget(DXGI_FORMAT format, uint32_t sample_count = 1) : RenderTargetBase(format), sample_count(sample_count) {}
        void init();
        void deinit() noexcept;
        static RenderTarget empty() noexcept { return RenderTarget(DXGI_FORMAT_UNKNOWN); }
        void ForceSizeResources(core::math::ivec2 const &size);
        void SizeResources(core::math::ivec2 const &size) override;
        uint32_t sample_count = 1;
    };

    class SwapchainRenderTarget final : public RenderTargetBase
    {
    public:
        using RenderTargetBase::RenderTargetBase;
        SwapchainRenderTarget() : RenderTargetBase(DXGI_FORMAT_UNKNOWN) {}
        void init(HWND hWnd, core::math::ivec2 const &window_size);
        void deinit() noexcept;
        void SizeResources(core::math::ivec2 const &size) override;
        [[nodiscard]] SwapChain1 const &swapchain() const noexcept { return swapchain_; }
        [[nodiscard]] SwapChain1 &swapchain() noexcept { return swapchain_; }
    private:
        SwapChain1 swapchain_ = nullptr;
    };
}