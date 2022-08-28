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
        static TextureId LoadTexture(std::filesystem::path const &path);
        static TextureId LoadCubemap(std::filesystem::path const &path);
        static TextureId LoadCubemap(std::array<std::filesystem::path, 6> const &cubemap_textures, bool generate_mipmaps = true);

        static inline [[nodiscard]] ID3D11ShaderResourceView *GetTextureView(TextureId texture) noexcept
        {
            if (auto it = textures_.find(texture);
                it != textures_.end())
            {
                return it->second;
            }
            return nullptr;
        }

    private:
        static TextureId current_id_;
        static std::unordered_map<TextureId, direct3d::ShaderResourceView> textures_;
        static std::unordered_map<TextureHash, TextureId> hash_ids_;
    };
}