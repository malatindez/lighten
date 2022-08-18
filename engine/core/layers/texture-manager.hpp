#pragma once
#include "layer.hpp"
#include "utils/utils.hpp"
#include "core/shader-compiler.hpp"
#include "direct3d11/d3d-common.hpp"
#include <filesystem>
#include "DirectXTex.h"
#include "DDSTextureLoader11.h"
#include "WICTextureLoader11.h"

namespace engine::core
{
    class Engine;
    class TextureManager final : public Layer
    {
    private:
        struct Texture final
        {
            direct3d::Resource texture_resource;
            direct3d::ShaderResourceView shader_resorce_view;
        };
    public:

        direct3d::ShaderResourceView GetTexture(std::filesystem::path const &path);

        [[nodiscard]] static std::shared_ptr<TextureManager> instance() noexcept { return instance_; }

        void OnAttach() override { utils::Assert(!attached_); attached_ = true; }
        void OnDetach() override { utils::Assert(attached_); attached_ = false; }

    private:
        friend class ::engine::core::Engine;

        static void Init() { utils::Assert(instance_ == nullptr); instance_ = std::shared_ptr<TextureManager>(new TextureManager()); }
        static void Deinit() { instance_ = nullptr; }
    private:
        TextureManager() {}

        // delete move & copy semantics
        TextureManager(TextureManager &&) = delete;
        TextureManager(TextureManager const &) = delete;
        TextureManager &operator=(TextureManager &&) = delete;
        TextureManager &operator=(TextureManager const &) = delete;

    private:
        static std::shared_ptr<TextureManager> instance_;

    private:
        std::unordered_map<std::filesystem::path, Texture> textures_;

        bool attached_ = false;
    };
}