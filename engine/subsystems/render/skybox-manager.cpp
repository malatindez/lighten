#pragma once
#include "skybox-manager.hpp"
#include "../core/shader-manager.hpp"
namespace engine
{
    std::unique_ptr<direct3d::ImmutableVertexBuffer<core::math::vec3>> SkyboxManager::cube_vb_;
    std::unique_ptr<direct3d::ImmutableIndexBuffer<uint32_t>> SkyboxManager::cube_ib_;
    std::unique_ptr<direct3d::DynamicUniformBuffer<core::math::mat4>> SkyboxManager::camera_model_buffer;
    render::GraphicsShaderProgram SkyboxManager::skybox_shader_;
    entt::entity SkyboxManager::LoadSkybox(entt::registry &registry, std::filesystem::path const &path)
    {
        entt::entity rv = registry.create();
        TextureId texture_id = std::numeric_limits<TextureId>::max();
        texture_id = core::TextureManager::LoadCubemap(path);
        registry.emplace<components::SkyboxComponent>(rv, components::SkyboxComponent{ .texture_id = texture_id });
        registry.emplace<components::TagComponent>(rv, components::TagComponent{ .tag = "Skybox" });
        return rv;
    }
    entt::entity SkyboxManager::LoadSkybox(entt::registry &registry, std::array<std::filesystem::path, 6> const &paths)
    {
        entt::entity rv = registry.create();
        TextureId texture_id = std::numeric_limits<TextureId>::max();
        texture_id = core::TextureManager::LoadCubemap(paths);
        registry.emplace<components::SkyboxComponent>(rv, components::SkyboxComponent{ .texture_id = texture_id });
        registry.emplace<components::TagComponent>(rv, components::TagComponent{ .tag = "Skybox" });
        return rv;
    }
    void SkyboxManager::RenderSkybox(components::SkyboxComponent const &skybox, core::math::vec3 const &camera_pos)
    {
        direct3d::api::devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        skybox_shader_.Bind();

        camera_model_buffer->Update(core::math::translate(core::math::mat4{ 1 }, camera_pos));
        camera_model_buffer->Bind(direct3d::ShaderType::VertexShader, 1);

        auto view = core::TextureManager::GetTextureView(skybox.texture_id);

        direct3d::api::devcon->RSSetState(direct3d::states::cull_none);
        direct3d::api::devcon4->PSSetSamplers(0, 1, &direct3d::states::point_wrap_sampler.ptr());
        direct3d::api::devcon4->OMSetDepthStencilState(direct3d::states::geq_depth, 0);
        direct3d::api::devcon4->OMSetBlendState(nullptr, nullptr, 0xffffffff); // use default blend mode (i.e. disable)

        direct3d::api::devcon->PSSetShaderResources(0, 1, &view);
        cube_vb_->Bind();
        cube_ib_->Bind();
        direct3d::api::devcon->DrawIndexed(cube_ib_->size(), 0, 0);
    }
    namespace
    {
        const std::array<core::math::vec3, 8> kSkyboxVertices =
        {
            core::math::vec3{-0.5f, -0.5f, 0.5f},
            core::math::vec3{0.5f, -0.5f, 0.5f},
            core::math::vec3{0.5f, 0.5f, 0.5f},
            core::math::vec3{-0.5f, 0.5f, 0.5f},
            core::math::vec3{-0.5f, -0.5f, -0.5f},
            core::math::vec3{0.5f, -0.5f, -0.5f},
            core::math::vec3{0.5f, 0.5f, -0.5f},
            core::math::vec3{-0.5f, 0.5f, -0.5f} };

        const std::array<uint32_t, 36> kSkyboxIndices =
        {
            // front
            0, 1, 2,
            2, 3, 0,
            // right
            1, 5, 6,
            6, 2, 1,
            // back
            7, 6, 5,
            5, 4, 7,
            // left
            4, 0, 3,
            3, 7, 4,
            // bottom
            4, 5, 1,
            1, 0, 4,
            // top
            3, 2, 6,
            6, 7, 3 };
    } // namespace

    void SkyboxManager::Init()
    {
        auto path = std::filesystem::current_path();

        std::vector<D3D11_INPUT_ELEMENT_DESC> d3d_input_desc{
         {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
        };

        auto vs = core::ShaderManager::instance()->CompileVertexShader(path / skybox_vertex_shader_path);
        auto ps = core::ShaderManager::instance()->CompilePixelShader(path / skybox_pixel_shader_path);
        auto il = std::make_shared<render::InputLayout>(vs->blob(), d3d_input_desc);

        skybox_shader_.SetVertexShader(vs).SetPixelShader(ps).SetInputLayout(il);

        cube_vb_ = std::make_unique<direct3d::ImmutableVertexBuffer<core::math::vec3>>(kSkyboxVertices);
        cube_ib_ = std::make_unique<direct3d::ImmutableIndexBuffer<uint32_t>>(kSkyboxIndices);
        camera_model_buffer = std::make_unique<direct3d::DynamicUniformBuffer<core::math::mat4>>();
    }
    void SkyboxManager::Deinit()
    {
        skybox_shader_.SetVertexShader(nullptr).SetPixelShader(nullptr).SetInputLayout(nullptr);
        cube_vb_ = nullptr;
        cube_ib_ = nullptr;
        camera_model_buffer = nullptr;
    }
} // namespace engine