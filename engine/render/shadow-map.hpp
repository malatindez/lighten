#pragma once
#include "direct3d11/direct3d11.hpp"
namespace engine::render
{
    class ShadowMap
    {
    public:
        ShadowMap() = default;
        ShadowMap(size_t size);
        ~ShadowMap() = default;
        ShadowMap(ShadowMap const &) = delete;
        ShadowMap(ShadowMap &&) = delete;
        ShadowMap &operator=(ShadowMap const &) = delete;
        ShadowMap &operator=(ShadowMap &&) = delete;
        void BindShaderResource(int slot)
        {
            direct3d::api().devcon4->PSSetShaderResources(slot, 1, &depth_texture_view_.ptr());
        }
        void Resize(size_t resolution, size_t size, bool cubemap = true);
        [[nodiscard]] inline size_t size() const noexcept { return size_; }
        [[nodiscard]] direct3d::ShaderResourceView depth_texture_view() const { return depth_texture_view_; }
        [[nodiscard]] direct3d::DepthStencil const &depth_stencil() const { return depth_stencil_; }
        [[nodiscard]] direct3d::DepthStencil &depth_stencil() { return depth_stencil_; }
    private:
        direct3d::ShaderResourceView depth_texture_view_ = nullptr;
        direct3d::DepthStencil depth_stencil_;
        size_t resolution_;
        size_t size_;
    };
}