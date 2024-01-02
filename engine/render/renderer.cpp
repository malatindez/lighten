#include "renderer.hpp"
#include "components/components.hpp"
#include "core/scene.hpp"
#include "subsystems/render/skybox-manager.hpp"
namespace lighten::render
{
    void Renderer::SkyboxRenderWrapper::OnRender(core::Scene *scene)
    {
        for (auto entity : scene->registry.view<components::SkyboxComponent>())
        {
            SkyboxManager::RenderSkybox(scene->registry.get<components::SkyboxComponent>(entity), *per_frame_ptr);
        }
    }
    void Renderer::FrameBegin(core::Scene *scene)
    {
        light_render_system_->OnRender(scene);
    }
    void Renderer::DeferredRender(core::Scene *scene, [[maybe_unused]] GBuffer const &buffer)
    {
        /*
        light_render_system_.RenderShadowMaps(scene);
        opaque_render_system_.Render(scene);
        dissolution_render_system_.Render(scene);
        emissive_render_system_.Render(scene);
        grass_render_system_.Render(scene);
        for (auto entity : scene->registry.view<components::SkyboxComponent>())
        {
            SkyboxManager::RenderSkybox(scene->registry.get<components::SkyboxComponent>(entity), per_frame);
        }
        particle_render_system_.Render(scene);*/
        opaque_render_system_->OnRender(scene);
        dissolution_render_system_->OnRender(scene);
        emissive_render_system_->OnRender(scene);
        grass_render_system_->OnRender(scene);
    }

    void Renderer::DeferredRender(core::Scene *scene, GBuffer const &buffer, ID3D11DepthStencilView *dsv, ID3D11ShaderResourceView *depth_srv, ID3D11ShaderResourceView *normals_srv)
    {
        decal_render_system_->OnRender(scene, buffer, dsv, depth_srv, normals_srv);
    }

    void Renderer::ForwardRender(core::Scene *scene, render::PerFrame const &per_frame, [[maybe_unused]] GBuffer const &buffer, [[maybe_unused]] ID3D11DepthStencilView *dsv, ID3D11ShaderResourceView *depth_srv, ID3D11ShaderResourceView *normals_srv)
    {
        skybox_render_pass_->per_frame_ptr = &per_frame;
        skybox_render_pass_->OnRender(scene);
        emissive_particle_render_system_->OnRender(depth_srv, normals_srv);
        particle_render_system_->OnRender(scene, depth_srv);
    }
    void Renderer::Tick(core::Scene *scene, float delta_time)
    {
        particle_render_system_->Tick(scene, delta_time);
    }
    void Renderer::Update(core::Scene *scene)
    {
        opaque_render_system_->Update(scene);
        dissolution_render_system_->Update(scene);
        emissive_render_system_->Update(scene);
        grass_render_system_->Update(scene);
        light_render_system_->Update(scene);
        emissive_particle_render_system_->Update(scene);
    }
    void Renderer::ScheduleInstanceUpdate()
    {
        opaque_render_system_->ScheduleInstanceUpdate();
        dissolution_render_system_->ScheduleInstanceUpdate();
        grass_render_system_->ScheduleInstanceUpdate();
        emissive_render_system_->ScheduleInstanceUpdate();
        light_render_system_->ScheduleInstanceUpdate();
        decal_render_system_->ScheduleInstanceUpdate();
    }
}