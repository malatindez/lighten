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
        static TextureId LoadTexture(uint8_t *data, size_t width, size_t height, size_t channels, bool generate_mips = true);
        static TextureId LoadTexture(std::filesystem::path const &path, bool generate_mipmaps = true);
        static TextureId LoadCubemap(std::filesystem::path const &path);
        static TextureId LoadCubemap(std::array<std::filesystem::path, 6> const &cubemap_textures, bool generate_mipmaps = true);
        [[nodiscard]] static inline ID3D11ShaderResourceView *const &GetTextureViewRef(TextureId texture)
        {
            if (auto it = instance_->textures_.find(texture);
                it != instance_->textures_.end())
            {
                return instance_->textures_[texture].ptr();
            }
            throw std::invalid_argument("Unknown handle");
        }
        [[nodiscard]] static inline ID3D11ShaderResourceView *GetTextureView(TextureId texture) noexcept
        {
            if (auto it = instance_->textures_.find(texture);
                it != instance_->textures_.end())
            {
                return it->second;
            }
            return nullptr;
        }
        [[nodiscard]] static inline ID3D11ShaderResourceView *GetTextureView(std::filesystem::path const &path, bool generate_mipmaps = true) noexcept
        {
            return GetTextureView(LoadTexture(path, generate_mipmaps));
        }
        [[nodiscard]] static inline TextureId GetTextureIdByPointer(ID3D11ShaderResourceView *ptr)
        {
            for (auto const &[id, texture] : instance_->textures_)
            {
                if (texture == ptr)
                {
                    return id;
                }
            }
            return kInvalidTextureId;
        }
        [[nodiscard]] static inline auto const &GetTextures() noexcept { return instance_->textures_; }
        [[nodiscard]] static inline auto const &GetTexturePaths() noexcept { return instance_->texture_paths_; }
        [[nodiscard]] static inline auto const &GetTextureHashes() noexcept { return instance_->texture_hashes_; }
    private:
        friend class ::engine::core::Engine;

        static void Init()
        {
            mal_toolkit::Assert(instance_ == nullptr);
            instance_ = std::unique_ptr<TextureManager>(new TextureManager());
        }
        static void Deinit() { instance_ = nullptr; }

    private:
        TextureManager() = default;

        // delete move & copy semantics
        TextureManager(TextureManager &&) = delete;
        TextureManager(TextureManager const &) = delete;
        TextureManager &operator=(TextureManager &&) = delete;
        TextureManager &operator=(TextureManager const &) = delete;

    private:
        static std::unique_ptr<TextureManager> instance_;

    private:
        TextureId current_id_ = 0;
        std::unordered_map<TextureId, direct3d::ShaderResourceView> textures_;
        std::unordered_map<TextureId, std::filesystem::path> texture_paths_;
        std::unordered_map<TextureHash, TextureId> texture_hashes_;
    };
}