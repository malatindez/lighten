#pragma once
#include "direct3d11/direct3d11.hpp"
namespace lighten::render
{
    class ShadowMap
    {
    public:
        ShadowMap() = default;
        ~ShadowMap() = default;
        ShadowMap(ShadowMap const &) = delete;
        ShadowMap(ShadowMap &&) = delete;
        ShadowMap &operator=(ShadowMap const &) = delete;
        ShadowMap &operator=(ShadowMap &&) = delete;
        void BindShaderResource(int slot)
        {
            direct3d::api().devcon4->PSSetShaderResources(slot, 1, &depth_texture_view_.ptr());
        }
        void Resize(uint32_t resolution, uint32_t size, bool cubemap = true);
        [[nodiscard]] inline uint32_t size() const noexcept { return size_; }
        [[nodiscard]] direct3d::ShaderResourceView depth_texture_view() const { return depth_texture_view_; }
        [[nodiscard]] direct3d::DepthStencil const &depth_stencil() const { return depth_stencil_; }
        [[nodiscard]] direct3d::DepthStencil &depth_stencil() { return depth_stencil_; }

    private:
        direct3d::ShaderResourceView depth_texture_view_ = nullptr;
        direct3d::DepthStencil depth_stencil_;
        uint32_t resolution_;
        uint32_t size_;
    };
}