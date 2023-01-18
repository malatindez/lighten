#include "renderer.hpp"
#include "components/components.hpp"
#include "core/scene.hpp"
#include "subsystems/render/skybox-manager.hpp"
namespace engine::render
{
    void Renderer::SkyboxRenderWrapper::OnRender(core::Scene *scene)
    {
        for (auto entity : scene->registry.view<components::SkyboxComponent>())
        {
            SkyboxManager::RenderSkybox(scene->registry.get<components::SkyboxComponent>(entity), *per_frame_ptr);
        }
    }
    void Renderer::Render(core::Scene *scene, render::PerFrame const &per_frame)
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
        skybox_render_pass_->per_frame_ptr = &per_frame;
        for (auto &render_pass : render_passes_)
        {
            render_pass->OnRender(scene);
        }
    }
    void Renderer::Tick(core::Scene *scene, float delta_time)
    {
        particle_render_system_->Tick(scene, delta_time);
    }
    void Renderer::Update(core::Scene *scene)
    {
        opaque_render_system_->Update(scene);
        dissolution_render_system_->Update(scene);
        grass_render_system_->Update(scene);
        emissive_render_system_->Update(scene);
        light_render_system_->Update(scene);
    }
    void Renderer::ScheduleOnInstancesUpdate()
    {
        opaque_render_system_->ScheduleOnInstancesUpdate();
        dissolution_render_system_->ScheduleOnInstancesUpdate();
        grass_render_system_->ScheduleOnInstancesUpdate();
        emissive_render_system_->ScheduleOnInstancesUpdate();
        light_render_system_->ScheduleOnInstancesUpdate();
    }
}