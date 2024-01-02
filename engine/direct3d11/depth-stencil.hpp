#pragma once

#include "direct3d11/d3d-debug.hpp"
#include "core/math.hpp"

namespace lighten::direct3d
{
    /// @todo finish documentation for DepthStencil class
    class DepthStencil
    {
    public:
        DepthStencil() noexcept = default;
        ~DepthStencil() noexcept = default;
        DepthStencil(DepthStencil &&) noexcept = default;
        DepthStencil &operator=(DepthStencil &&) noexcept = default;
        DepthStencil(DepthStencil const &) noexcept = default;
        DepthStencil &operator=(DepthStencil const &) noexcept = default;

        [[nodiscard]] ID3D11Texture2D const *const &depth_buffer() const noexcept { return depth_buffer_.ptr(); }
        [[nodiscard]] ID3D11DepthStencilView const *const &depth_stencil_view() const noexcept { return depth_stencil_view_.ptr(); }
        [[nodiscard]] ID3D11Texture2D *const &depth_buffer() noexcept { return depth_buffer_.ptr(); }
        [[nodiscard]] ID3D11DepthStencilView *const &depth_stencil_view() noexcept { return depth_stencil_view_.ptr(); }
        void deinit()
        {
            depth_buffer_.reset();
            depth_stencil_view_.reset();
        }
        void init(D3D11_TEXTURE2D_DESC *depth_buffer_desc, D3D11_DEPTH_STENCIL_VIEW_DESC *depth_stencil_view_desc)
        {
            AlwaysAssert(api().device->CreateTexture2D(depth_buffer_desc, nullptr, &depth_buffer_.reset()), "Failed to create depth stencil texture");
            AlwaysAssert(api().device->CreateDepthStencilView(depth_buffer_.ptr(), depth_stencil_view_desc, &depth_stencil_view_.reset()), "Failed to create depth stencil view");
        }
        void init(glm::ivec2 const &size)
        {
            // initialize depth buffer
            D3D11_TEXTURE2D_DESC depth_buffer_desc;
            ZeroMemory(&depth_buffer_desc, sizeof(D3D11_TEXTURE2D_DESC));
            depth_buffer_desc.Width = size.x;
            depth_buffer_desc.Height = size.y;
            depth_buffer_desc.MipLevels = 1;
            depth_buffer_desc.ArraySize = 1;
            depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            depth_buffer_desc.SampleDesc.Count = 1;
            depth_buffer_desc.SampleDesc.Quality = 0;
            depth_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
            depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
            depth_buffer_desc.CPUAccessFlags = 0;
            depth_buffer_desc.MiscFlags = 0;
            // initialize depth stencil view
            D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
            ZeroMemory(&depth_stencil_view_desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
            depth_stencil_view_desc.Format = depth_buffer_desc.Format;
            depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            depth_stencil_view_desc.Flags = 0;
            init(&depth_buffer_desc, &depth_stencil_view_desc);
        }

    private:
        Texture2D depth_buffer_;
        DepthStencilView depth_stencil_view_;
    };
}