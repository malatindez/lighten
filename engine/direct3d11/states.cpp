#include "states.hpp"
namespace
{
    D3D11_DEPTH_STENCIL_DESC CreateDepthState(bool enable, bool write_enable)
    {
        D3D11_DEPTH_STENCIL_DESC desc = {};

        desc.DepthEnable = enable ? TRUE : FALSE;
        desc.DepthWriteMask = write_enable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
        desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;

        desc.StencilEnable = FALSE;
        desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

        desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

        desc.BackFace = desc.FrontFace;
        return desc;
    }

    D3D11_RASTERIZER_DESC CreateRasterizerState(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode)
    {
        D3D11_RASTERIZER_DESC desc = {};

        desc.CullMode = cullMode;
        desc.FillMode = fillMode;
        desc.DepthClipEnable = TRUE;
        desc.MultisampleEnable = TRUE;

        return desc;
    }

    D3D11_SAMPLER_DESC CreateSamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode)
    {
        D3D11_SAMPLER_DESC desc = {};

        desc.Filter = filter;

        desc.AddressU = addressMode;
        desc.AddressV = addressMode;
        desc.AddressW = addressMode;

        desc.MaxAnisotropy = D3D11_MAX_MAXANISOTROPY;

        desc.MinLOD = 0;
        desc.MaxLOD = D3D11_FLOAT32_MAX;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

        return desc;
    }
}
namespace engine::direct3d
{
    std::unique_ptr<States> States::instance_ = nullptr;
    States::States()
    {
        D3D11_RASTERIZER_DESC raster_desc = CreateRasterizerState(D3D11_CULL_NONE, D3D11_FILL_SOLID);
        api().device->CreateRasterizerState(&raster_desc, &cull_none.reset());

        raster_desc = CreateRasterizerState(D3D11_CULL_BACK, D3D11_FILL_SOLID);
        api().device->CreateRasterizerState(&raster_desc, &cull_back.reset());

        D3D11_DEPTH_STENCIL_DESC ds_desc = CreateDepthState(true, true);
        api().device->CreateDepthStencilState(&ds_desc, &geq_depth.reset());
        ds_desc = CreateDepthState(true, false);
        api().device->CreateDepthStencilState(&ds_desc, &geq_depth_no_write.reset());
        ds_desc = CreateDepthState(false, false);
        api().device->CreateDepthStencilState(&ds_desc, &no_depth_no_write.reset());

        D3D11_SAMPLER_DESC sampler_desc = CreateSamplerState(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
        api().device5->CreateSamplerState(&sampler_desc, &point_wrap_sampler.reset());

        sampler_desc = CreateSamplerState(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP);
        api().device5->CreateSamplerState(&sampler_desc, &point_clamp_sampler.reset());

        sampler_desc = CreateSamplerState(D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
        api().device5->CreateSamplerState(&sampler_desc, &bilinear_wrap_sampler.reset());

        sampler_desc = CreateSamplerState(D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP);
        api().device5->CreateSamplerState(&sampler_desc, &bilinear_clamp_sampler.reset());

        sampler_desc = CreateSamplerState(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
        api().device5->CreateSamplerState(&sampler_desc, &trilinear_wrap_sampler.reset());

        sampler_desc = CreateSamplerState(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
        api().device5->CreateSamplerState(&sampler_desc, &trilinear_clamp_sampler.reset());

        sampler_desc = CreateSamplerState(D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP);
        api().device5->CreateSamplerState(&sampler_desc, &anisotropic_wrap_sampler.reset());

        sampler_desc = CreateSamplerState(D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_CLAMP);
        api().device5->CreateSamplerState(&sampler_desc, &anisotropic_clamp_sampler.reset());

        sampler_desc = CreateSamplerState(D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
        api().device5->CreateSamplerState(&sampler_desc, &comparison_point_wrap_sampler.reset());

        sampler_desc = CreateSamplerState(D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP);
        api().device5->CreateSamplerState(&sampler_desc, &comparison_point_clamp_sampler.reset());

        sampler_desc = CreateSamplerState(D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
        api().device5->CreateSamplerState(&sampler_desc, &comparison_linear_wrap_sampler.reset());

        sampler_desc = CreateSamplerState(D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
        api().device5->CreateSamplerState(&sampler_desc, &comparison_linear_clamp_sampler.reset());

        sampler_desc = CreateSamplerState(D3D11_FILTER_COMPARISON_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP);
        api().device5->CreateSamplerState(&sampler_desc, &comparison_anisotropic_wrap_sampler.reset());

        sampler_desc = CreateSamplerState(D3D11_FILTER_COMPARISON_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_CLAMP);
        api().device5->CreateSamplerState(&sampler_desc, &comparison_anisotropic_clamp_sampler.reset());
    }
}