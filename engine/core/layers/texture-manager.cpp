#include "texture-manager.hpp"
namespace engine::core
{
    std::shared_ptr<TextureManager> TextureManager::instance_;

    direct3d::ShaderResourceView TextureManager::GetTexture(std::filesystem::path const &path)
    {
        utils::AlwaysAssert(path.is_absolute(), "Paths provided to texture manager should be absolute!");
        utils::Assert(path.has_extension(), "Path to texture should have an extension");
        if (auto it = textures_.find(path);
            it != textures_.end())
        {
            return it->second.shader_resorce_view;
        }
        Texture texture;
        if (utils::as_lowercase(path.extension().string()) == ".dds")
        {
            utils::AlwaysAssert(
                SUCCEEDED(DirectX::CreateDDSTextureFromFile(direct3d::api::device5, direct3d::api::devcon4, path.wstring().c_str(), &texture.texture_resource.reset(), &texture.shader_resorce_view.reset())),
                "Failed to load dds texture from file @" + path.string());
        }
        else if (utils::as_lowercase(path.extension().string()) == ".hdr")
        {
            utils::AlwaysAssert(
                false,
                "HDR textures are not implemented" + path.string());
        }
        else if (utils::as_lowercase(path.extension().string()) == ".tga")
        {
            DirectX::TexMetadata metadata;
            auto image = std::make_unique<DirectX::ScratchImage>();
            direct3d::AlwaysAssert(DirectX::LoadFromTGAFile(path.wstring().c_str(),
                                                            DirectX::TGA_FLAGS_NONE,
                                                            &metadata, *image),
                                   "Failed to load TGA texture from file @ " + path.string());
            D3D11_TEXTURE2D_DESC tdesc;
            ZeroMemory(&tdesc, sizeof(D3D11_TEXTURE2D_DESC));
            tdesc.Format = image->GetImage(0, 0, 0)->format;
            tdesc.SampleDesc.Count = 1;
            tdesc.SampleDesc.Quality = 0;
            tdesc.ArraySize = (uint32_t)metadata.arraySize;
            tdesc.MipLevels = (uint32_t)metadata.mipLevels;
            tdesc.Width = (uint32_t)metadata.width;
            tdesc.Height = (uint32_t)metadata.height;
            tdesc.Usage = D3D11_USAGE_IMMUTABLE;
            tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            tdesc.CPUAccessFlags = 0;
            tdesc.MiscFlags = metadata.miscFlags;

            D3D11_SUBRESOURCE_DATA srd;
            srd = D3D11_SUBRESOURCE_DATA {};
            srd.pSysMem = image->GetImage(0, 0, 0)->pixels;
            srd.SysMemPitch = image->GetImage(0, 0, 0)->rowPitch;
            srd.SysMemSlicePitch = 0;

            ID3D11Texture2D *texture_ = nullptr;
            direct3d::AlwaysAssert(direct3d::api::device5->CreateTexture2D(&tdesc, &srd, &texture_),
                                   "Failed to create 2d texture: ");
            texture.texture_resource = texture_;
            D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
            srvd.Format = metadata.format;
            srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvd.Texture2D.MipLevels = metadata.mipLevels;
            srvd.Texture2D.MostDetailedMip = 0;
            direct3d::AlwaysAssert(direct3d::api::device5->CreateShaderResourceView(texture_, &srvd, &texture.shader_resorce_view.reset()),
                                   "Failed to create shader resource view: ");
        }
        else
        {
            direct3d::AlwaysAssert(
                DirectX::CreateWICTextureFromFile(direct3d::api::device5,
                                                  direct3d::api::devcon4,
                                                  path.wstring().c_str(),
                                                  &texture.texture_resource.reset(),
                                                  &texture.shader_resorce_view.reset()),
                "Failed to load wic texture from file @" + path.string());
        }
        textures_.emplace(std::make_pair(path, texture));
        return texture.shader_resorce_view;
    }

}