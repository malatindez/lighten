#include "d3d-common.hpp"
namespace engine::direct3d
{
    ID3D11RenderTargetView * const null_rtv = nullptr;
    ID3D11DepthStencilView * const null_dsv = nullptr;
    ID3D11ShaderResourceView * const null_srv = nullptr;

    enum class ShaderType : uint8_t
    {
        VertexShader = 0,
        PixelShader = 1,
        HullShader = 2,
        DomainShader = 3,
        GeometryShader = 4,
        ComputeShader = 5,
        None = 6
    };

    using SwapChain1 = d3d_resource_wrapper<IDXGISwapChain1>;
    using RenderTargetView1 = d3d_resource_wrapper<ID3D11RenderTargetView1>;
    using RenderTargetView = d3d_resource_wrapper<ID3D11RenderTargetView>;
    using DepthStencilView = d3d_resource_wrapper<ID3D11DepthStencilView>;
    using DepthStencilState = d3d_resource_wrapper<ID3D11DepthStencilState>;
    using Texture2D = d3d_resource_wrapper<ID3D11Texture2D>;
    using Buffer = d3d_resource_wrapper<ID3D11Buffer>;
    using InputLayout = d3d_resource_wrapper<ID3D11InputLayout>;
    using RasterizerState1 = d3d_resource_wrapper<ID3D11RasterizerState1>;
    using RasterizerState = d3d_resource_wrapper<ID3D11RasterizerState>;
    using SamplerState = d3d_resource_wrapper<ID3D11SamplerState>;
    using Resource = d3d_resource_wrapper<ID3D11Resource>;
    using VertexShader = d3d_resource_wrapper<ID3D11VertexShader>;
    using PixelShader = d3d_resource_wrapper<ID3D11PixelShader>;
    using HullShader = d3d_resource_wrapper<ID3D11HullShader>;
    using DomainShader = d3d_resource_wrapper<ID3D11DomainShader>;
    using GeometryShader = d3d_resource_wrapper<ID3D11GeometryShader>;
    using ComputeShader = d3d_resource_wrapper<ID3D11ComputeShader>;
    using Blob = d3d_resource_wrapper<ID3D10Blob>;
    using Buffer = d3d_resource_wrapper<ID3D11Buffer>;
    using ShaderResourceView = d3d_resource_wrapper<ID3D11ShaderResourceView>;
    using UnorderedAccessView = d3d_resource_wrapper<ID3D11UnorderedAccessView>;
    using Resource = d3d_resource_wrapper<ID3D11Resource>;
    using BlendState = d3d_resource_wrapper<ID3D11BlendState>;

    using Factory = d3d_resource_wrapper<IDXGIFactory>;
    using Factory5 = d3d_resource_wrapper<IDXGIFactory5>;
    using Device = d3d_resource_wrapper<ID3D11Device>;
    using Device5 = d3d_resource_wrapper<ID3D11Device5>;
    using DeviceContext = d3d_resource_wrapper<ID3D11DeviceContext>;
    using DeviceContext4 = d3d_resource_wrapper<ID3D11DeviceContext4>;
#if defined(_DEBUG)
    using Debug = d3d_resource_wrapper<ID3D11Debug>;
    using DebugInfoQueue = d3d_resource_wrapper<ID3D11InfoQueue>;
#endif
}