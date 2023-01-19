#pragma once
#include "api.hpp"
namespace engine::direct3d
{
    class States;
    /// @brief utilities for States class
    /// @details This namespace contains all the classes and functions related to the states that are not meant to be used by the user
    namespace _state_detail
    {
        /// @brief The state_readonly_wrapper class is a wrapper for readonly state d3d resources
        /// @tparam T The type of the resource
        /// @details This class is used to wrap readonly d3d resources, so that they can be used in the States class.
        /// These resources are protected from accidental modification, and can only be modified by the States class.
        template <typename T>
        class state_readonly_wrapper final : public readonly_d3d_resource_wrapper<T>
        {
            friend class engine::direct3d::States;
            using readonly_d3d_resource_wrapper<T>::operator=;
            using readonly_d3d_resource_wrapper<T>::readonly_d3d_resource_wrapper;
        };
    } // namespace _state_detail

    using ReadOnlyRasterizerState = _state_detail::state_readonly_wrapper<ID3D11RasterizerState>;
    using ReadOnlySamplerState = _state_detail::state_readonly_wrapper<ID3D11SamplerState>;
    using ReadOnlyDepthStencilState = _state_detail::state_readonly_wrapper<ID3D11DepthStencilState>;
    using ReadOnlyBlendState = _state_detail::state_readonly_wrapper<ID3D11BlendState>;
    /// @brief The class that contains all the states
    /// @details This class is a singleton, and can be accessed using the instance() function
    class States
    {
    public:
        /// @brief The default rasterizer state, no culling
        /// @details This state is equivalent to the following D3D11_RASTERIZER_DESC:
        /// * FillMode = D3D11_FILL_SOLID
        /// * CullMode = D3D11_CULL_NONE
        /// * FrontCounterClockwise = false
        /// * DepthBias = 0
        /// * DepthBiasClamp = 0
        /// * SlopeScaledDepthBias = 0
        /// * DepthClipEnable = true
        /// * ScissorEnable = false
        /// * MultisampleEnable = false
        /// * AntialiasedLineEnable = false
        ReadOnlyRasterizerState cull_none;
        /// @brief The rasterizer state that culls back faces
        /// @details This state is equivalent to the following D3D11_RASTERIZER_DESC:
        /// * FillMode = D3D11_FILL_SOLID
        /// * CullMode = D3D11_CULL_BACK
        /// * FrontCounterClockwise = false
        /// * DepthBias = 0
        /// * DepthBiasClamp = 0
        /// * SlopeScaledDepthBias = 0
        /// * DepthClipEnable = true
        /// * ScissorEnable = false
        /// * MultisampleEnable = false
        /// * AntialiasedLineEnable = false
        ReadOnlyRasterizerState cull_back;
        /// @brief The rasterizer state that culls front faces
        /// @details This state is equivalent to the following D3D11_RASTERIZER_DESC:
        /// * FillMode = D3D11_FILL_SOLID
        /// * CullMode = D3D11_CULL_FRONT
        /// * FrontCounterClockwise = false
        /// * DepthBias = 0
        /// * DepthBiasClamp = 0
        /// * SlopeScaledDepthBias = 0
        /// * DepthClipEnable = true
        /// * ScissorEnable = false
        /// * MultisampleEnable = false
        /// * AntialiasedLineEnable = false
        ReadOnlyRasterizerState cull_front;
        /// @brief Greater or equal depth test, depth write enabled
        /// @details This state is equivalent to the following D3D11_DEPTH_STENCIL_DESC:
        /// * DepthEnable = true
        /// * DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL
        /// * DepthFunc = D3D11_COMPARISON_GREATER_EQUAL
        /// * StencilEnable = false
        /// * StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK
        /// * StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK
        /// * FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP
        /// * FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP
        /// * FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP
        /// * FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS
        /// * BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP
        /// * BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP
        /// * BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP
        /// * BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS
        ReadOnlyDepthStencilState geq_depth;
        /// @brief Greater or equal depth test, depth write disabled
        /// @details This state is equivalent to the following D3D11_DEPTH_STENCIL_DESC:
        /// * DepthEnable = true
        /// * DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO
        /// * DepthFunc = D3D11_COMPARISON_GREATER_EQUAL
        /// * StencilEnable = false
        /// * StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK
        /// * StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK
        /// * FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP
        /// * FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP
        /// * FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP
        /// * FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS
        /// * BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP
        /// * BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP
        /// * BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP
        /// * BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS
        ReadOnlyDepthStencilState geq_depth_no_write;
        
        /// @brief no depth test, depth write enabled
        /// @details This state is equivalent to the following D3D11_DEPTH_STENCIL_DESC:
        /// * DepthEnable = false
        /// * DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL
        /// * DepthFunc = D3D11_COMPARISON_ALWAYS
        /// * StencilEnable = false
        /// * StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK
        /// * StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK
        /// * FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP
        /// * FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP
        /// * FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP
        /// * FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS
        /// * BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP
        /// * BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP
        /// * BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP
        /// * BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS
        ReadOnlyDepthStencilState no_depth_write;
        /// @brief no depth test, depth write disabled
        /// @details This state is equivalent to the following D3D11_DEPTH_STENCIL_DESC:
        /// * DepthEnable = false
        /// * DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO
        /// * DepthFunc = D3D11_COMPARISON_ALWAYS
        /// * StencilEnable = false
        /// * StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK
        /// * StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK
        /// * FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP
        /// * FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP
        /// * FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP
        /// * FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS
        /// * BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP
        /// * BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP
        /// * BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP
        /// * BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS
        ReadOnlyDepthStencilState no_depth_no_write;
        /// @brief no depth test, depth write disabled, stencil test enabled
        /// @details This state is equivalent to the following D3D11_DEPTH_STENCIL_DESC:
        /// * DepthEnable = false
        /// * DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO
        /// * DepthFunc = D3D11_COMPARISON_ALWAYS
        /// * StencilEnable = true
        /// * StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK
        /// * StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK
        /// * FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP
        /// * FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP
        /// * FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP
        /// * FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS
        /// * BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP
        /// * BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP
        /// * BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP
        /// * BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS
        ReadOnlyDepthStencilState stencil_test;

        /// @brief Point wrap sampler state
        /// @details This state is equivalent to the following D3D11_SAMPLER_DESC:
        /// * Filter = D3D11_FILTER_MIN_MAG_MIP_POINT
        /// * AddressU = D3D11_TEXTURE_ADDRESS_WRAP
        /// * AddressV = D3D11_TEXTURE_ADDRESS_WRAP
        /// * AddressW = D3D11_TEXTURE_ADDRESS_WRAP
        /// * MipLODBias = 0
        /// * MaxAnisotropy = D3D11_MAX_MAXANISOTROPY
        /// * ComparisonFunc = D3D11_COMPARISON_NEVER
        /// * BorderColor[4] = { 0, 0, 0, 0 }
        /// * MinLOD = 0
        /// * MaxLOD = D3D11_FLOAT32_MAX
        ReadOnlySamplerState point_wrap_sampler;
        /// @brief Point clamp sampler state
        /// @details This state is equivalent to the following D3D11_SAMPLER_DESC:
        /// * Filter = D3D11_FILTER_MIN_MAG_MIP_POINT
        /// * AddressU = D3D11_TEXTURE_ADDRESS_CLAMP
        /// * AddressV = D3D11_TEXTURE_ADDRESS_CLAMP
        /// * AddressW = D3D11_TEXTURE_ADDRESS_CLAMP
        /// * MipLODBias = 0
        /// * MaxAnisotropy = D3D11_MAX_MAXANISOTROPY
        /// * ComparisonFunc = D3D11_COMPARISON_NEVER
        /// * BorderColor[4] = { 0, 0, 0, 0 }
        /// * MinLOD = 0
        /// * MaxLOD = D3D11_FLOAT32_MAX
        ReadOnlySamplerState point_clamp_sampler;
        /// @brief Bilinear wrap sampler state
        /// @details This state is equivalent to the following D3D11_SAMPLER_DESC:
        /// * Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR
        /// * AddressU = D3D11_TEXTURE_ADDRESS_WRAP
        /// * AddressV = D3D11_TEXTURE_ADDRESS_WRAP
        /// * AddressW = D3D11_TEXTURE_ADDRESS_WRAP
        /// * MipLODBias = 0
        /// * MaxAnisotropy = D3D11_MAX_MAXANISOTROPY
        /// * ComparisonFunc = D3D11_COMPARISON_NEVER
        /// * BorderColor[4] = { 0, 0, 0, 0 }
        /// * MinLOD = 0
        /// * MaxLOD = D3D11_FLOAT32_MAX
        ReadOnlySamplerState bilinear_wrap_sampler;
        /// @brief Bilinear clamp sampler state
        /// @details This state is equivalent to the following D3D11_SAMPLER_DESC:
        /// * Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR
        /// * AddressU = D3D11_TEXTURE_ADDRESS_CLAMP
        /// * AddressV = D3D11_TEXTURE_ADDRESS_CLAMP
        /// * AddressW = D3D11_TEXTURE_ADDRESS_CLAMP
        /// * MipLODBias = 0
        /// * MaxAnisotropy = D3D11_MAX_MAXANISOTROPY
        /// * ComparisonFunc = D3D11_COMPARISON_NEVER
        /// * BorderColor[4] = { 0, 0, 0, 0 }
        /// * MinLOD = 0
        /// * MaxLOD = D3D11_FLOAT32_MAX
        ReadOnlySamplerState bilinear_clamp_sampler;
        /// @brief Anisotropic wrap sampler state
        /// @details This state is equivalent to the following D3D11_SAMPLER_DESC:
        /// * Filter = D3D11_FILTER_ANISOTROPIC
        /// * AddressU = D3D11_TEXTURE_ADDRESS_WRAP
        /// * AddressV = D3D11_TEXTURE_ADDRESS_WRAP
        /// * AddressW = D3D11_TEXTURE_ADDRESS_WRAP
        /// * MipLODBias = 0
        /// * MaxAnisotropy = D3D11_MAX_MAXANISOTROPY
        /// * ComparisonFunc = D3D11_COMPARISON_NEVER
        /// * BorderColor[4] = { 0, 0, 0, 0 }
        /// * MinLOD = 0
        /// * MaxLOD = D3D11_FLOAT32_MAX
        ReadOnlySamplerState anisotropic_wrap_sampler;
        /// @brief Anisotropic clamp sampler state
        /// @details This state is equivalent to the following D3D11_SAMPLER_DESC:
        /// * Filter = D3D11_FILTER_ANISOTROPIC
        /// * AddressU = D3D11_TEXTURE_ADDRESS_CLAMP
        /// * AddressV = D3D11_TEXTURE_ADDRESS_CLAMP
        /// * AddressW = D3D11_TEXTURE_ADDRESS_CLAMP
        /// * MipLODBias = 0
        /// * MaxAnisotropy = D3D11_MAX_MAXANISOTROPY
        /// * ComparisonFunc = D3D11_COMPARISON_NEVER
        /// * BorderColor[4] = { 0, 0, 0, 0 }
        /// * MinLOD = 0
        /// * MaxLOD = D3D11_FLOAT32_MAX
        ReadOnlySamplerState anisotropic_clamp_sampler;
        /// @brief Comparison point wrap sampler state
        /// @details This state is equivalent to the following D3D11_SAMPLER_DESC:
        /// * Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT
        /// * AddressU = D3D11_TEXTURE_ADDRESS_WRAP
        /// * AddressV = D3D11_TEXTURE_ADDRESS_WRAP
        /// * AddressW = D3D11_TEXTURE_ADDRESS_WRAP
        /// * MipLODBias = 0
        /// * MaxAnisotropy = D3D11_MAX_MAXANISOTROPY
        /// * ComparisonFunc = D3D11_COMPARISON_LESS
        /// * BorderColor[4] = { 0, 0, 0, 0 }
        /// * MinLOD = 0
        /// * MaxLOD = D3D11_FLOAT32_MAX
        ReadOnlySamplerState comparison_point_wrap_sampler;
        /// @brief Comparison point clamp sampler state
        /// @details This state is equivalent to the following D3D11_SAMPLER_DESC:
        /// * Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT
        /// * AddressU = D3D11_TEXTURE_ADDRESS_CLAMP
        /// * AddressV = D3D11_TEXTURE_ADDRESS_CLAMP
        /// * AddressW = D3D11_TEXTURE_ADDRESS_CLAMP
        /// * MipLODBias = 0
        /// * MaxAnisotropy = D3D11_MAX_MAXANISOTROPY
        /// * ComparisonFunc = D3D11_COMPARISON_LESS
        /// * BorderColor[4] = { 0, 0, 0, 0 }
        /// * MinLOD = 0
        /// * MaxLOD = D3D11_FLOAT32_MAX
        ReadOnlySamplerState comparison_point_clamp_sampler;
        /// @brief Comparison bilinear wrap sampler state
        /// @details This state is equivalent to the following D3D11_SAMPLER_DESC:
        /// * Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR
        /// * AddressU = D3D11_TEXTURE_ADDRESS_WRAP
        /// * AddressV = D3D11_TEXTURE_ADDRESS_WRAP
        /// * AddressW = D3D11_TEXTURE_ADDRESS_WRAP
        /// * MipLODBias = 0
        /// * MaxAnisotropy = D3D11_MAX_MAXANISOTROPY
        /// * ComparisonFunc = D3D11_COMPARISON_LESS
        /// * BorderColor[4] = { 0, 0, 0, 0 }
        /// * MinLOD = 0
        /// * MaxLOD = D3D11_FLOAT32_MAX
        ReadOnlySamplerState comparison_linear_wrap_sampler;
        /// @brief Comparison bilinear clamp sampler state
        /// @details This state is equivalent to the following D3D11_SAMPLER_DESC:
        /// * Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR
        /// * AddressU = D3D11_TEXTURE_ADDRESS_CLAMP
        /// * AddressV = D3D11_TEXTURE_ADDRESS_CLAMP
        /// * AddressW = D3D11_TEXTURE_ADDRESS_CLAMP
        /// * MipLODBias = 0
        /// * MaxAnisotropy = D3D11_MAX_MAXANISOTROPY
        /// * ComparisonFunc = D3D11_COMPARISON_LESS
        /// * BorderColor[4] = { 0, 0, 0, 0 }
        /// * MinLOD = 0
        /// * MaxLOD = D3D11_FLOAT32_MAX
        ReadOnlySamplerState comparison_linear_clamp_sampler;
        /// @brief Comparison anisotropic wrap sampler state
        /// @details This state is equivalent to the following D3D11_SAMPLER_DESC:
        /// * Filter = D3D11_FILTER_COMPARISON_ANISOTROPIC
        /// * AddressU = D3D11_TEXTURE_ADDRESS_WRAP
        /// * AddressV = D3D11_TEXTURE_ADDRESS_WRAP
        /// * AddressW = D3D11_TEXTURE_ADDRESS_WRAP
        /// * MipLODBias = 0
        /// * MaxAnisotropy = D3D11_MAX_MAXANISOTROPY
        /// * ComparisonFunc = D3D11_COMPARISON_LESS
        /// * BorderColor[4] = { 0, 0, 0, 0 }
        /// * MinLOD = 0
        /// * MaxLOD = D3D11_FLOAT32_MAX
        ReadOnlySamplerState comparison_anisotropic_wrap_sampler;
        /// @brief Comparison anisotropic clamp sampler state
        /// @details This state is equivalent to the following D3D11_SAMPLER_DESC:
        /// * Filter = D3D11_FILTER_COMPARISON_ANISOTROPIC
        /// * AddressU = D3D11_TEXTURE_ADDRESS_CLAMP
        /// * AddressV = D3D11_TEXTURE_ADDRESS_CLAMP
        /// * AddressW = D3D11_TEXTURE_ADDRESS_CLAMP
        /// * MipLODBias = 0
        /// * MaxAnisotropy = D3D11_MAX_MAXANISOTROPY
        /// * ComparisonFunc = D3D11_COMPARISON_LESS
        /// * BorderColor[4] = { 0, 0, 0, 0 }
        /// * MinLOD = 0
        /// * MaxLOD = D3D11_FLOAT32_MAX
        ReadOnlySamplerState comparison_anisotropic_clamp_sampler;
        /// @brief Additive blend state with alpha blending
        /// @details This state is equivalent to the following D3D11_BLEND_DESC:
        /// * AlphaToCoverageEnable = FALSE
        /// * IndependentBlendEnable = FALSE
        /// * RenderTarget[0].BlendEnable = TRUE
        /// * RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA
        /// * RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA
        /// * RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD
        /// * RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE
        /// * RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE
        /// * RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD
        /// * RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL
        ReadOnlyBlendState additive_blend_state_alpha;
        /// @brief Additive blend state without alpha blending
        /// @details This state is equivalent to the following D3D11_BLEND_DESC:
        /// * AlphaToCoverageEnable = FALSE
        /// * IndependentBlendEnable = FALSE
        /// * RenderTarget[0].BlendEnable = TRUE
        /// * RenderTarget[0].SrcBlend = D3D11_BLEND_ONE
        /// * RenderTarget[0].DestBlend = D3D11_BLEND_ONE
        /// * RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD
        /// * RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE
        /// * RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE
        /// * RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD
        /// * RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL
        ReadOnlyBlendState additive_blend_state_no_alpha;
        /// @brief Alpha to coverage blend state
        /// @details This state is equivalent to the following D3D11_BLEND_DESC:
        /// * AlphaToCoverageEnable = TRUE
        /// * IndependentBlendEnable = FALSE
        /// * RenderTarget[0].BlendEnable = FALSE
        /// * RenderTarget[0].SrcBlend = D3D11_BLEND_ONE
        /// * RenderTarget[0].DestBlend = D3D11_BLEND_ZERO
        /// * RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD
        /// * RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE
        /// * RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO
        /// * RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD
        /// * RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL
        ReadOnlyBlendState alpha_to_coverage_blend_state;

        /// @brief instance of the states class
        /// @details This is a singleton class, so this is the only instance of the class
        [[nodiscard]] static inline States &instance() { utils::Assert(instance_ != nullptr, "States not initialized"); return *instance_; }
    private:
        friend class Api;
        States();
        States(const States &) = delete;
        States(States &&) = delete;
        States &operator=(const States &) = delete;
        States &operator=(States &&) = delete;
        /// @brief Initializes the states class
        /// @note This function can only be called by the Api class

        static void Init()
        {
            utils::Assert(instance_ == nullptr, "States already initialized");
            instance_ = std::unique_ptr<States>(new States());
        }
        /// @brief Deinitializes the states class
        /// @note This function can only be called by the Api class
        static void Deinit() { instance_.reset(); }
        /// @brief instance of the states class
        /// @details This is a singleton class, so this is the only instance of the class
        static std::unique_ptr<States> instance_;
    };
    /// @brief Returns the instance of the states class
    [[nodiscard]] inline States &states() { return States::instance(); }
}