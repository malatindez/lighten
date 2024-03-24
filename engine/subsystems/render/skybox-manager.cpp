#include "skybox-manager.hpp"
#include "../core/shader-manager.hpp"
namespace lighten
{
    render::GraphicsShaderProgram SkyboxManager::skybox_shader_;
    std::unique_ptr<direct3d::DynamicUniformBuffer<glm::mat3x4>> SkyboxManager::skybox_buffer_;

    entt::entity SkyboxManager::LoadSkybox(entt::registry &registry, std::filesystem::path const &path)
    {
        entt::entity rv = registry.create();
        registry.emplace<components::GameObject>(rv).name = "Skybox";
        TextureId texture_id = std::numeric_limits<TextureId>::max();
        texture_id = core::TextureManager::LoadCubemap(path);
        registry.emplace<components::SkyboxComponent>(rv, components::SkyboxComponent{.texture_id = texture_id});
        return rv;
    }
    entt::entity SkyboxManager::LoadSkybox(entt::registry &registry, std::array<std::filesystem::path, 6> const &paths)
    {
        entt::entity rv = registry.create();
        TextureId texture_id = std::numeric_limits<TextureId>::max();
        texture_id = core::TextureManager::LoadCubemap(paths);
        registry.emplace<components::SkyboxComponent>(rv, components::SkyboxComponent{.texture_id = texture_id});
        return rv;
    }
    void SkyboxManager::RenderSkybox(components::SkyboxComponent const &skybox, render::PerFrame const &per_frame)
    {
        direct3d::api().devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        glm::mat4 modified_inv_view_projection = per_frame.inv_view;
        modified_inv_view_projection[3][0] = modified_inv_view_projection[3][1] = modified_inv_view_projection[3][2] = 0;
        modified_inv_view_projection = modified_inv_view_projection * per_frame.inv_projection;
        glm::vec4 bl4 = modified_inv_view_projection * glm::vec4(-1, -1, 1, 1);
        glm::vec4 br4 = modified_inv_view_projection * glm::vec4(1, -1, 1, 1);
        glm::vec4 tl4 = modified_inv_view_projection * glm::vec4(-1, 1, 1, 1);
        glm::vec4 right4 = br4 - bl4;
        glm::vec4 up4 = tl4 - bl4;
        glm::mat3x4 modified_inv_view_projection_3x4;
        modified_inv_view_projection_3x4[0] = bl4;
        modified_inv_view_projection_3x4[1] = right4;
        modified_inv_view_projection_3x4[2] = up4;
//        float* t = reinterpret_cast<float*>(&modified_inv_view_projection_3x4[0][0]);
        skybox_buffer_->Update(modified_inv_view_projection_3x4);
        skybox_buffer_->Bind(direct3d::ShaderType::VertexShader, 2);
        skybox_shader_.Bind();
        auto view = core::TextureManager::GetTextureView(skybox.texture_id);

        direct3d::api().devcon4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        direct3d::api().devcon4->RSSetState(direct3d::states().cull_none);

        direct3d::api().devcon4->OMSetBlendState(nullptr, nullptr, 0xffffffff); // use default blend mode (i.e. disable)
        direct3d::api().devcon4->OMSetDepthStencilState(direct3d::states().no_depth_stencil_read, 0);

        direct3d::api().devcon4->PSSetShaderResources(0, 1, &view);

        direct3d::api().devcon->Draw(3, 0);
        skybox_shader_.Unbind();
    }

    void SkyboxManager::Init()
    {
        auto path = std::filesystem::current_path();

        auto vs = core::ShaderManager::instance()->CompileVertexShader(path / skybox_vs_shader_path);
        auto ps = core::ShaderManager::instance()->CompilePixelShader(path / skybox_ps_shader_path);

        skybox_shader_.SetVertexShader(vs).SetPixelShader(ps);
        skybox_buffer_ = std::make_unique<direct3d::DynamicUniformBuffer<glm::mat3x4>>();
    }
    void SkyboxManager::Deinit()
    {
        skybox_shader_.SetVertexShader(nullptr).SetPixelShader(nullptr).SetInputLayout(nullptr);
        skybox_buffer_.reset();
    }
} // namespace lighten