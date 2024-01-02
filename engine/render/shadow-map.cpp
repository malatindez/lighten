#include "shadow-map.hpp"
namespace lighten::render
{
    void ShadowMap::Resize(uint32_t resolution, uint32_t size, bool cubemap)
    {
        if (resolution == resolution_ && size == size_)
            return;
        if (resolution == 0 || size == 0)
        {
            return;
        }
        resolution_ = resolution;
        size_ = size;
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = resolution;
        desc.Height = resolution;
        desc.MipLevels = 1;
        desc.ArraySize = (cubemap ? 6 : 1) * size;
        desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc{};
        dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
        dsv_desc.Texture2DArray.MipSlice = 0;
        dsv_desc.Texture2DArray.ArraySize = desc.ArraySize;

        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
        srv_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        srv_desc.Texture2DArray.MostDetailedMip = 0;
        srv_desc.Texture2DArray.MipLevels = 1;
        srv_desc.Texture2DArray.FirstArraySlice = 0;
        srv_desc.Texture2DArray.ArraySize = desc.ArraySize;
        if (cubemap)
        {
            desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
            dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
            dsv_desc.Texture2DArray.ArraySize = 6 * size;
            srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
            srv_desc.TextureCubeArray.MostDetailedMip = 0;
            srv_desc.TextureCubeArray.MipLevels = 1;
            srv_desc.TextureCubeArray.First2DArrayFace = 0;
            srv_desc.TextureCubeArray.NumCubes = size;
        }
        depth_stencil_.init(&desc, &dsv_desc);
        direct3d::AlwaysAssert(direct3d::api().device5->CreateShaderResourceView(depth_stencil_.depth_buffer(), &srv_desc, &depth_texture_view_.reset()));
    }
}