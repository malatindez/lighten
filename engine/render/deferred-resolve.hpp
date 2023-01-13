#pragma once
#include "../subsystems/render/post-processing.hpp"
#include "shader-program.hpp"
#include "../subsystems/core/shader-manager.hpp"
#include "render/common.hpp"
namespace engine::core
{
    class Scene;
}
namespace engine::render
{
    class DeferredResolve
    {
    public:
        struct PointLightInstance
        {
            core::math::mat4 transform_matrix;
            core::math::vec3 color;
            float radius;
            core::math::vec3 position;
            // if > 0, then this is a shadow casting light
            int32_t shadow_map_index;
        };
        struct SpotLightInstance
        {
            core::math::mat4 transform_matrix;
            core::math::vec3 color;
            float radius;
            core::math::vec3 direction;
            float inner_cutoff;
            core::math::vec3 position;
            float outer_cutoff;
            // if > 0, then this is a shadow casting light
            int32_t shadow_map_index;
        };
        struct DeferredPerFrame
        {
            uint32_t prefiltered_map_mip_levels;
            core::math::vec3 padding;
        };
        DeferredResolve(direct3d::RenderTarget &hdr_render_target) : hdr_render_target_{ hdr_render_target }
        {
            auto path = std::filesystem::current_path();
            {
                auto vs = core::ShaderManager::instance()->CompileVertexShader(path / vs_environment_lights_shader_path);
                auto ps = core::ShaderManager::instance()->CompilePixelShader(path / ps_environment_lights_shader_path);
                environment_shader_.SetVertexShader(vs).SetPixelShader(ps);
            }
            {
                std::vector<D3D11_INPUT_ELEMENT_DESC> d3d_input_desc{
                    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
                    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
                    {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
                    {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
                    {"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
                    { "ROWX",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA,  1},
                    { "ROWY",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
                    { "ROWZ",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
                    { "ROWW",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
                    { "LIGHT_COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
                    { "LIGHT_RADIUS",    0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
                    { "LIGHT_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
                    { "LIGHT_SHADOW_MAP_INDEX", 0, DXGI_FORMAT_R32_SINT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
                };
                auto vs = core::ShaderManager::instance()->CompileVertexShader(path / vs_volume_point_lights_shader_path);
                auto il = std::make_shared<render::InputLayout>(vs->blob(), d3d_input_desc);
                auto ps = core::ShaderManager::instance()->CompilePixelShader(path / ps_volume_point_lights_shader_path);
                volume_point_shader_.SetVertexShader(vs).SetPixelShader(ps).SetInputLayout(il);
            }
            {
                std::vector<D3D11_INPUT_ELEMENT_DESC> d3d_input_desc{
                    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                    {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                    {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                    {"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                    { "ROWX",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
                    { "ROWY",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
                    { "ROWZ",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
                    { "ROWW",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
                    { "LIGHT_COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
                    { "LIGHT_RADIUS",    0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
                    { "LIGHT_DIRECTION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
                    { "LIGHT_INNER_CUTOFF", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
                    { "LIGHT_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
                    { "LIGHT_OUTER_CUTOFF", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
                    { "LIGHT_SHADOW_MAP_INDEX", 0, DXGI_FORMAT_R32_SINT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
                };
                auto vs = core::ShaderManager::instance()->CompileVertexShader(path / vs_volume_spot_lights_shader_path);
                auto il = std::make_shared<render::InputLayout>(vs->blob(), d3d_input_desc);
                auto ps = core::ShaderManager::instance()->CompilePixelShader(path / ps_volume_spot_lights_shader_path);
                volume_spot_shader_.SetVertexShader(vs).SetPixelShader(ps).SetInputLayout(il);
            }
        }
        direct3d::RenderTargetBase &Process(render::GBuffer &g_buffer, core::Scene *scene, ID3D11DepthStencilView *depth_target);

        void SetIrradianceTexture(ID3D11ShaderResourceView *texture) { irradiance_texture_ = texture; }
        void SetPrefilteredTexture(ID3D11ShaderResourceView *texture) { prefiltered_texture_ = texture; }
        void SetBrdfTexture(ID3D11ShaderResourceView *texture) { brdf_texture_ = texture; }
        [[nodiscard]] ID3D11ShaderResourceView *GetIrradianceTexture() const { return irradiance_texture_; }
        [[nodiscard]] ID3D11ShaderResourceView *GetPrefilteredTexture() const { return prefiltered_texture_; }
        [[nodiscard]] ID3D11ShaderResourceView *GetBrdfTexture() const { return brdf_texture_; }

    private:
        static constexpr auto vs_environment_lights_shader_path = "assets/shaders/post-processing/deferred-resolve-environment-lights-vs.hlsl";
        static constexpr auto ps_environment_lights_shader_path = "assets/shaders/post-processing/deferred-resolve-environment-lights-ps.hlsl";
        static constexpr auto vs_volume_point_lights_shader_path = "assets/shaders/post-processing/deferred-resolve-volume-point-lights-vs.hlsl";
        static constexpr auto ps_volume_point_lights_shader_path = "assets/shaders/post-processing/deferred-resolve-volume-point-lights-ps.hlsl";
        static constexpr auto vs_volume_spot_lights_shader_path = "assets/shaders/post-processing/deferred-resolve-volume-spot-lights-vs.hlsl";
        static constexpr auto ps_volume_spot_lights_shader_path = "assets/shaders/post-processing/deferred-resolve-volume-spot-lights-ps.hlsl";

        GraphicsShaderProgram environment_shader_;
        GraphicsShaderProgram volume_point_shader_;
        GraphicsShaderProgram volume_spot_shader_;

        direct3d::RenderTarget &hdr_render_target_;
        direct3d::DynamicUniformBuffer<DeferredPerFrame> per_frame_buffer_;
        direct3d::DynamicVertexBuffer<PointLightInstance> point_light_instance_buffer_;
        direct3d::DynamicVertexBuffer<SpotLightInstance> spot_light_instance_buffer_;

        ID3D11ShaderResourceView *irradiance_texture_;
        ID3D11ShaderResourceView *prefiltered_texture_;
        ID3D11ShaderResourceView *brdf_texture_;
    };
}