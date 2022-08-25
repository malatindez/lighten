#pragma once
#include "DDSTextureLoader11.h"
#include "DirectXTex.h"
#include "WICTextureLoader11.h"
#include "include/engine-pch.hpp"

namespace engine::core
{
    class Engine;
    class TextureManager final
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

    private:
        friend class ::engine::core::Engine;

        static void Init()
        {
            utils::Assert(instance_ == nullptr);
            instance_ = std::shared_ptr<TextureManager>(new TextureManager());
        }
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
        std::unordered_map<size_t, Texture> textures_;
    };
}