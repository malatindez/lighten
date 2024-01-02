#include "texture-manager.hpp"
namespace lighten::core
{
    std::unique_ptr<TextureManager> TextureManager::instance_ = nullptr;

    namespace
    {
        constexpr uint32_t MipmapLevels(uint32_t width, uint32_t height)
        {
            uint32_t levels = 1;
            while ((width | height) >> levels)
                ++levels;
            return levels;
        }

        direct3d::ShaderResourceView LoadTextureFromPath(std::filesystem::path const &path, bool mipmaps)
        {
            static direct3d::Resource texture_resource;
            direct3d::ShaderResourceView shader_resource_view;
            std::string extension = mal_toolkit::as_lowercase(path.extension().string());
            if (extension == ".dds")
            {
                direct3d::AlwaysAssert(
                    DirectX::CreateDDSTextureFromFile(direct3d::api().device5, direct3d::api().devcon4, path.wstring().c_str(), &texture_resource.reset(), &shader_resource_view.reset()),
                    "Failed to load DDS texture from file @" + path.string());
            }
            else if (extension == ".hdr")
            {
                mal_toolkit::AlwaysAssert(
                    false,
                    "HDR textures are not implemented" + path.string());
            }
            else if (extension == ".tga")
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
                tdesc.MipLevels = mipmaps ? MipmapLevels((uint32_t)metadata.width, (uint32_t)metadata.height) : (uint32_t)metadata.mipLevels;
                tdesc.Width = (uint32_t)metadata.width;
                tdesc.Height = (uint32_t)metadata.height;
                tdesc.Usage = D3D11_USAGE_DEFAULT;
                tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                tdesc.CPUAccessFlags = 0;
                tdesc.BindFlags = mipmaps ? D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET : D3D11_BIND_SHADER_RESOURCE;
                tdesc.MiscFlags = mipmaps ? D3D11_RESOURCE_MISC_GENERATE_MIPS | metadata.miscFlags : metadata.miscFlags;

                D3D11_SUBRESOURCE_DATA srd;
                srd = D3D11_SUBRESOURCE_DATA{};
                srd.pSysMem = image->GetImage(0, 0, 0)->pixels;
                srd.SysMemPitch = (uint32_t)image->GetImage(0, 0, 0)->rowPitch;
                srd.SysMemSlicePitch = 0;

                direct3d::Texture2D texture;
                if (mipmaps)
                {
                    direct3d::AlwaysAssert(direct3d::api().device->CreateTexture2D(&tdesc, nullptr, &texture.reset()), "Failed to create texture");
                    direct3d::api().devcon->UpdateSubresource(texture,
                                                              D3D11CalcSubresource(0, 0, tdesc.MipLevels),
                                                              nullptr,
                                                              srd.pSysMem,
                                                              srd.SysMemPitch,
                                                              1);
                }
                else
                {
                    direct3d::AlwaysAssert(direct3d::api().device->CreateTexture2D(&tdesc, &srd, &texture.reset()),
                                           "Failed to create texture resource");
                }

                D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
                srv_desc.Format = tdesc.Format;
                srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                srv_desc.TextureCube.MostDetailedMip = 0;
                srv_desc.TextureCube.MipLevels = std::numeric_limits<UINT>::max();

                direct3d::AlwaysAssert(direct3d::api().device->CreateShaderResourceView(texture, &srv_desc, &shader_resource_view.ptr()),
                                       "Failed to create shader resource view");

                if (mipmaps)
                    direct3d::api().devcon->GenerateMips(shader_resource_view);
            }
            else
            {
                direct3d::AlwaysAssert(
                    DirectX::CreateWICTextureFromFile(direct3d::api().device5,
                                                      direct3d::api().devcon4,
                                                      path.wstring().c_str(),
                                                      &texture_resource.reset(),
                                                      &shader_resource_view.reset()),
                    "Failed to load WIC texture from file @" + path.string());
            }
            return shader_resource_view;
        }

        std::unique_ptr<DirectX::ScratchImage> LoadImageFromPath(std::filesystem::path const &path)
        {
            std::string extension = mal_toolkit::as_lowercase(path.extension().string());
            auto image = std::make_unique<DirectX::ScratchImage>();
            DirectX::TexMetadata metadata;
            if (extension == ".tga")
            {
                direct3d::AlwaysAssert(DirectX::LoadFromTGAFile(path.wstring().c_str(),
                                                                DirectX::TGA_FLAGS_NONE,
                                                                &metadata, *image),
                                       "Failed to load TGA texture from file @ " + path.string());
            }
            else
            {
                direct3d::AlwaysAssert(DirectX::LoadFromWICFile(path.wstring().c_str(),
                                                                DirectX::WIC_FLAGS_NONE,
                                                                &metadata, *image),
                                       "Failed to load WIC texture from file @ " + path.string());
            }
            return image;
        }
    }

    TextureId TextureManager::LoadTexture(uint8_t *data, size_t width, size_t height, size_t channels, bool generate_mipmaps)
    {
        D3D11_TEXTURE2D_DESC desc{};
        desc.ArraySize = 1;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = generate_mipmaps ? D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET : D3D11_BIND_SHADER_RESOURCE;
        desc.MiscFlags = generate_mipmaps ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
        desc.Width = (uint32_t)width;
        desc.Height = (uint32_t)height;
        desc.MipLevels = generate_mipmaps ? MipmapLevels((uint32_t)width, (uint32_t)height) : 1;
        switch (channels)
        {
        case 1:
            desc.Format = DXGI_FORMAT_R8_UNORM;
            break;
        case 2:
            desc.Format = DXGI_FORMAT_R8G8_UNORM;
            break;
        case 3:
            desc.Format = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
            break;
        case 4:
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        default:
            mal_toolkit::AlwaysAssert(false, "Invalid number of channels");
        }
        direct3d::Texture2D texture;
        D3D11_SUBRESOURCE_DATA srd;
        srd.pSysMem = data;
        srd.SysMemPitch = (uint32_t)width * (uint32_t)channels;
        srd.SysMemSlicePitch = 0;
        if (generate_mipmaps)
        {
            direct3d::AlwaysAssert(direct3d::api().device->CreateTexture2D(&desc, nullptr, &texture.reset()), "Failed to create texture");
            direct3d::api().devcon->UpdateSubresource(texture,
                                                      D3D11CalcSubresource(0, 0, desc.MipLevels),
                                                      nullptr,
                                                      srd.pSysMem,
                                                      srd.SysMemPitch,
                                                      1);
        }
        else
        {
            direct3d::AlwaysAssert(direct3d::api().device->CreateTexture2D(&desc, &srd, &texture.reset()), "Failed to create texture resource");
        }
        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
        srv_desc.Format = desc.Format;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srv_desc.Texture2D.MipLevels = std::numeric_limits<UINT>::max();
        srv_desc.Texture2D.MostDetailedMip = 0;
        direct3d::ShaderResourceView shader_resource_view;
        direct3d::AlwaysAssert(direct3d::api().device->CreateShaderResourceView(texture, &srv_desc, &shader_resource_view.reset()), "Failed to create shader resource view");
        if (generate_mipmaps)
            direct3d::api().devcon->GenerateMips(shader_resource_view);
        instance_->textures_.emplace(std::make_pair(instance_->current_id_, shader_resource_view));
        return instance_->current_id_++;
    }
    TextureId TextureManager::LoadTexture(std::filesystem::path const &input_path, bool generate_mipmaps)
    {
        mal_toolkit::AlwaysAssert(input_path.is_absolute(), "Paths provided to texture manager should be absolute!");
        mal_toolkit::Assert(input_path.has_extension(), "Path to texture should have an extension");
        auto path = input_path.parent_path().parent_path() / "dds" / (input_path.stem().string() + ".dds");
        if (!std::filesystem::exists(path))
        {
            path = input_path;
        }
        // For some reason the check above does pass if file does not exist. 
#ifdef _DEBUG
        {
            std::ifstream file(path.c_str());
            if (!file)
            {
                path = input_path;
            }
        }
#endif

        if (auto it = instance_->texture_hashes_.find(std::filesystem::hash_value(path));
            it != instance_->texture_hashes_.end())
        {
            return it->second;
        }
        auto shader_resource_view = LoadTextureFromPath(path, generate_mipmaps);
        std::filesystem::path relative_path = path.lexically_relative(std::filesystem::current_path());
        if (relative_path.empty())
            relative_path = path;
        instance_->texture_paths_.emplace(std::make_pair(instance_->current_id_, relative_path));
        instance_->textures_.emplace(std::make_pair(instance_->current_id_, shader_resource_view));
        instance_->texture_hashes_.emplace(std::make_pair(std::filesystem::hash_value(path), instance_->current_id_));
        return instance_->current_id_++;
    }

    TextureId TextureManager::LoadCubemap(std::filesystem::path const &path)
    {
        std::string extension = mal_toolkit::as_lowercase(path.extension().string());
        mal_toolkit::Assert(extension == ".dds", "Single-file cubemap has to be in DDS format");
        if (auto it = instance_->texture_hashes_.find(std::filesystem::hash_value(path));
            it != instance_->texture_hashes_.end())
        {
            return it->second;
        }

        static direct3d::Resource texture_resource;
        direct3d::ShaderResourceView shader_resource_view;
        direct3d::AlwaysAssert(
            DirectX::CreateDDSTextureFromFileEx(direct3d::api().device5,
                                                direct3d::api().devcon4,
                                                path.wstring().c_str(),
                                                0,
                                                D3D11_USAGE_IMMUTABLE,
                                                D3D11_BIND_SHADER_RESOURCE,
                                                0,
                                                D3D11_RESOURCE_MISC_TEXTURECUBE,
                                                DirectX::DDS_LOADER_DEFAULT,
                                                &texture_resource.reset(),
                                                &shader_resource_view.reset()),
            "Failed to load DDS texture from file @" + path.string());

        instance_->textures_.emplace(std::make_pair(instance_->current_id_, shader_resource_view));
        std::filesystem::path relative_path = path.lexically_relative(std::filesystem::current_path());
        if (relative_path.empty())
            relative_path = path;
        instance_->texture_paths_.emplace(std::make_pair(instance_->current_id_, relative_path));
        instance_->texture_hashes_.emplace(std::make_pair(std::filesystem::hash_value(path), instance_->current_id_));
        return instance_->current_id_++;
    }
    TextureId TextureManager::LoadCubemap(std::array<std::filesystem::path, 6> const &cubemap_textures, bool generate_mipmaps)
    {
        D3D11_TEXTURE2D_DESC desc{};
        desc.ArraySize = 6;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = generate_mipmaps ? D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET : D3D11_BIND_SHADER_RESOURCE;
        desc.MiscFlags = generate_mipmaps ? D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS : D3D11_RESOURCE_MISC_TEXTURECUBE;

        std::vector<std::unique_ptr<DirectX::ScratchImage>> images{};
        std::vector<D3D11_SUBRESOURCE_DATA> subresource_data_array;

        for (auto const &path : cubemap_textures)
        {
            images.emplace_back(LoadImageFromPath(path));

            D3D11_SUBRESOURCE_DATA subresource_data{};
            subresource_data.pSysMem = images.back()->GetImage(0, 0, 0)->pixels;
            subresource_data.SysMemPitch = (uint32_t)images.back()->GetImage(0, 0, 0)->rowPitch;
            subresource_data_array.push_back(subresource_data);
        }

        desc.Width = (uint32_t)images[0]->GetImage(0, 0, 0)->width;
        desc.Height = (uint32_t)images[0]->GetImage(0, 0, 0)->height;
        desc.MipLevels = generate_mipmaps ? MipmapLevels(desc.Width, desc.Height) : 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

        direct3d::Texture2D texture;

        if (generate_mipmaps)
        {
            direct3d::AlwaysAssert(direct3d::api().device->CreateTexture2D(&desc, nullptr, &texture.reset()), "Failed to create texture");
            for (uint32_t i = 0; i < cubemap_textures.size(); ++i)
                direct3d::api().devcon->UpdateSubresource(texture,
                                                          D3D11CalcSubresource(0, i, desc.MipLevels),
                                                          nullptr,
                                                          subresource_data_array[i].pSysMem,
                                                          subresource_data_array[i].SysMemPitch, 1);
        }
        else
        {
            direct3d::AlwaysAssert(direct3d::api().device->CreateTexture2D(&desc, subresource_data_array.data(), &texture.reset()),
                                   "Failed to create texture resource");
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
        srv_desc.Format = desc.Format;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
        srv_desc.TextureCube.MostDetailedMip = 0;
        srv_desc.TextureCube.MipLevels = std::numeric_limits<UINT>::max();

        direct3d::ShaderResourceView shader_resource_view;
        direct3d::AlwaysAssert(direct3d::api().device->CreateShaderResourceView(texture, &srv_desc, &shader_resource_view.ptr()),
                               "Failed to create shader resource view");

        if (generate_mipmaps)
            direct3d::api().devcon->GenerateMips(shader_resource_view);
        instance_->textures_.emplace(std::make_pair(instance_->current_id_, shader_resource_view));
        return instance_->current_id_++;
    }
}