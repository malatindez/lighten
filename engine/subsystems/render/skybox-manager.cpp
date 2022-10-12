#include "skybox-manager.hpp"
#include "../core/shader-manager.hpp"
namespace engine
{
    render::GraphicsShaderProgram SkyboxManager::skybox_shader_;
    std::unique_ptr<direct3d::DynamicUniformBuffer<core::math::mat4x3>> SkyboxManager::skybox_buffer_;

    entt::entity SkyboxManager::LoadSkybox(entt::registry &registry, std::filesystem::path const &path)
    {
        entt::entity rv = registry.create();
        TextureId texture_id = std::numeric_limits<TextureId>::max();
        texture_id = core::TextureManager::LoadCubemap(path);
        registry.emplace<components::SkyboxComponent>(rv, components::SkyboxComponent{ .texture_id = texture_id });
        return rv;
    }
    entt::entity SkyboxManager::LoadSkybox(entt::registry &registry, std::array<std::filesystem::path, 6> const &paths)
    {
        entt::entity rv = registry.create();
        TextureId texture_id = std::numeric_limits<TextureId>::max();
        texture_id = core::TextureManager::LoadCubemap(paths);
        registry.emplace<components::SkyboxComponent>(rv, components::SkyboxComponent{ .texture_id = texture_id });
        return rv;
    }
    void SkyboxManager::RenderSkybox(components::SkyboxComponent const &skybox, render::PerFrame const &per_frame)
    {
        direct3d::api().devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        core::math::mat4 modified_inv_view_projection = per_frame.inv_view;
        modified_inv_view_projection[3][0] = modified_inv_view_projection[3][1] = modified_inv_view_projection[3][2] = 0;
        modified_inv_view_projection = modified_inv_view_projection * per_frame.inv_projection;
        core::math::vec4 bl4 = modified_inv_view_projection * core::math::vec4(-1, -1, 1, 1);
        core::math::vec4 br4 = modified_inv_view_projection * core::math::vec4(1, -1, 1, 1);
        core::math::vec4 tl4 = modified_inv_view_projection * core::math::vec4(-1, 1, 1, 1);
        core::math::vec4 right4 = br4 - bl4;
        core::math::vec4 up4 = tl4 - bl4;
        skybox_buffer_->Update(core::math::mat4x3{ bl4, right4, up4 });
        skybox_buffer_->Bind(direct3d::ShaderType::VertexShader, 1);
        skybox_shader_.Bind();
        auto view = core::TextureManager::GetTextureView(skybox.texture_id);

        direct3d::api().devcon->RSSetState(direct3d::states().cull_none);
        direct3d::api().devcon4->PSSetSamplers(0, 1, &direct3d::states().bilinear_wrap_sampler.ptr());
        direct3d::api().devcon4->OMSetDepthStencilState(direct3d::states().geq_depth_no_write, 0);
        direct3d::api().devcon4->OMSetBlendState(nullptr, nullptr, 0xffffffff); // use default blend mode (i.e. disable)

        direct3d::api().devcon->PSSetShaderResources(0, 1, &view);

        direct3d::api().devcon->Draw(3, 0);
    }

    void SkyboxManager::Init()
    {
        auto path = std::filesystem::current_path();

        auto vs = core::ShaderManager::instance()->CompileVertexShader(path / skybox_vs_shader_path);
        auto ps = core::ShaderManager::instance()->CompilePixelShader(path / skybox_ps_shader_path);

        skybox_shader_.SetVertexShader(vs).SetPixelShader(ps);
        skybox_buffer_ = std::make_unique<direct3d::DynamicUniformBuffer<core::math::mat4x3>>();
    }
    void SkyboxManager::Deinit()
    {
        skybox_shader_.SetVertexShader(nullptr).SetPixelShader(nullptr).SetInputLayout(nullptr);
        skybox_buffer_.reset();
    }
} // namespace engine