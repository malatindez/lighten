#pragma once
#include "DDSTextureLoader11.h"
#include "DirectXTex.h"
#include "WICTextureLoader11.h"
#include "include/engine-pch.hpp"

namespace engine::core
{
    class Engine;
    inline constexpr TextureId const kInvalidTextureId = std::numeric_limits<size_t>::max();
    class TextureManager final
    {
    private:
        using TextureHash = size_t;
    public:
        static TextureId LoadTexture(std::filesystem::path const &path, bool generate_mipmaps = true);
        static TextureId LoadCubemap(std::filesystem::path const &path);
        static TextureId LoadCubemap(std::array<std::filesystem::path, 6> const &cubemap_textures, bool generate_mipmaps = true);

        static inline [[nodiscard]] ID3D11ShaderResourceView *GetTextureView(TextureId texture) noexcept
        {
            if (auto it = instance_->textures_.find(texture);
                it != instance_->textures_.end())
            {
                return it->second;
            }
            return nullptr;
        }

    private:
        friend class ::engine::core::Engine;

        static void Init()
        {
            utils::Assert(instance_ == nullptr);
            instance_ = std::unique_ptr<TextureManager>(new TextureManager());
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
        static std::unique_ptr<TextureManager> instance_;

    private:
        TextureId current_id_;
        std::unordered_map<TextureId, direct3d::ShaderResourceView> textures_;
        std::unordered_map<TextureHash, TextureId> hash_ids_;
    };
}