#include "states.hpp"
#include "d3d-debug.hpp"
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
        desc.MultisampleEnable = FALSE;

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

    D3D11_SAMPLER_DESC CreateComparisonState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode, D3D11_COMPARISON_FUNC comparisonFunc)
    {
        D3D11_SAMPLER_DESC desc = CreateSamplerState(filter, addressMode);
        desc.ComparisonFunc = comparisonFunc;
        return desc;
    }
}
namespace engine::direct3d
{
    std::unique_ptr<States> States::instance_ = nullptr;
    States::States()
    {
        D3D11_RASTERIZER_DESC raster_desc = CreateRasterizerState(D3D11_CULL_NONE, D3D11_FILL_SOLID);
        AlwaysAssert(api().device->CreateRasterizerState(&raster_desc, &cull_none.reset()), "Failed to create cull none rasterizer state");

        raster_desc = CreateRasterizerState(D3D11_CULL_BACK, D3D11_FILL_SOLID);
        AlwaysAssert(api().device->CreateRasterizerState(&raster_desc, &cull_back.reset()), "Failed to create cull back rasterizer state");
        raster_desc = CreateRasterizerState(D3D11_CULL_FRONT, D3D11_FILL_SOLID);
        AlwaysAssert(api().device->CreateRasterizerState(&raster_desc, &cull_front.reset()), "Failed to create cull front rasterizer state");

        D3D11_DEPTH_STENCIL_DESC ds_desc = CreateDepthState(true, true);
        AlwaysAssert(api().device->CreateDepthStencilState(&ds_desc, &geq_depth.reset()), "Failed to create depth state");
        ds_desc = CreateDepthState(true, false);
        AlwaysAssert(api().device->CreateDepthStencilState(&ds_desc, &geq_depth_no_write.reset()), "Failed to create depth state");
        ds_desc = CreateDepthState(false, false);
        AlwaysAssert(api().device->CreateDepthStencilState(&ds_desc, &no_depth_no_write.reset()), "Failed to create depth state");
        ds_desc = CreateDepthState(true, true);
        ds_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
        AlwaysAssert(api().device->CreateDepthStencilState(&ds_desc, &no_depth_write.reset()), "Failed to create depth state");
        ZeroMemory(&ds_desc, sizeof(ds_desc));
        ds_desc = CreateDepthState(true, true);
        ds_desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
        ds_desc.StencilEnable = true;
        ds_desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
        ds_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        ds_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        ds_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
        ds_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_REPLACE;
        ds_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_REPLACE;
        ds_desc.BackFace = ds_desc.FrontFace;
        AlwaysAssert(api().device->CreateDepthStencilState(&ds_desc, &geq_depth_write_stencil_replace.reset()), "Failed to create depth state");
        ds_desc = CreateDepthState(true, true);
        ds_desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
        ds_desc.StencilEnable = true;
        ds_desc.StencilWriteMask = 0;
        ds_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        ds_desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
        ds_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        ds_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        ds_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        ds_desc.BackFace = ds_desc.FrontFace;
        AlwaysAssert(api().device->CreateDepthStencilState(&ds_desc, &geq_depth_write_stencil_read.reset()), "Failed to create depth state");


        D3D11_SAMPLER_DESC sampler_desc = CreateSamplerState(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
        AlwaysAssert(api().device5->CreateSamplerState(&sampler_desc, &point_wrap_sampler.reset()), "Failed to create sampler state");

        sampler_desc = CreateSamplerState(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP);
        AlwaysAssert(api().device5->CreateSamplerState(&sampler_desc, &point_clamp_sampler.reset()), "Failed to create sampler state");

        sampler_desc = CreateSamplerState(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
        AlwaysAssert(api().device5->CreateSamplerState(&sampler_desc, &bilinear_wrap_sampler.reset()), "Failed to create sampler state");

        sampler_desc = CreateSamplerState(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
        AlwaysAssert(api().device5->CreateSamplerState(&sampler_desc, &bilinear_clamp_sampler.reset()), "Failed to create sampler state");

        sampler_desc = CreateSamplerState(D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP);
        AlwaysAssert(api().device5->CreateSamplerState(&sampler_desc, &anisotropic_wrap_sampler.reset()), "Failed to create sampler state");

        sampler_desc = CreateSamplerState(D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_CLAMP);
        AlwaysAssert(api().device5->CreateSamplerState(&sampler_desc, &anisotropic_clamp_sampler.reset()), "Failed to create sampler state");

        sampler_desc = CreateComparisonState(D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_LESS_EQUAL);
        AlwaysAssert(api().device5->CreateSamplerState(&sampler_desc, &comparison_point_wrap_sampler.reset()), "Failed to create sampler state");

        sampler_desc = CreateComparisonState(D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_COMPARISON_LESS_EQUAL);
        AlwaysAssert(api().device5->CreateSamplerState(&sampler_desc, &comparison_point_clamp_sampler.reset()), "Failed to create sampler state");

        sampler_desc = CreateComparisonState(D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_LESS_EQUAL);
        AlwaysAssert(api().device5->CreateSamplerState(&sampler_desc, &comparison_linear_wrap_sampler.reset()), "Failed to create sampler state");

        sampler_desc = CreateComparisonState(D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_COMPARISON_LESS_EQUAL);
        AlwaysAssert(api().device5->CreateSamplerState(&sampler_desc, &comparison_linear_clamp_sampler.reset()), "Failed to create sampler state");

        sampler_desc = CreateComparisonState(D3D11_FILTER_COMPARISON_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_LESS_EQUAL);
        AlwaysAssert(api().device5->CreateSamplerState(&sampler_desc, &comparison_anisotropic_wrap_sampler.reset()), "Failed to create sampler state");

        sampler_desc = CreateComparisonState(D3D11_FILTER_COMPARISON_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_COMPARISON_LESS_EQUAL);
        AlwaysAssert(api().device5->CreateSamplerState(&sampler_desc, &comparison_anisotropic_clamp_sampler.reset()), "Failed to create sampler state");

        D3D11_BLEND_DESC blend_desc;
        ZeroMemory(&blend_desc, sizeof(blend_desc));
        blend_desc.RenderTarget[0].BlendEnable = true;
        blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
        blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        AlwaysAssert(api().device->CreateBlendState(&blend_desc, &additive_blend_state_alpha.reset()), "Failed to create additive blend state");

        ZeroMemory(&blend_desc, sizeof(blend_desc));
        blend_desc.RenderTarget[0].BlendEnable = true;
        blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
        blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
        blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        AlwaysAssert(api().device->CreateBlendState(&blend_desc, &additive_blend_state_no_alpha.reset()), "Failed to create additive blend state");

        ZeroMemory(&blend_desc, sizeof(blend_desc));
        blend_desc.AlphaToCoverageEnable = true;
        blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
        blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
        blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        AlwaysAssert(api().device->CreateBlendState(&blend_desc, &alpha_to_coverage_blend_state.reset()), "Failed to create alpha to coverage blend state");
    }
}