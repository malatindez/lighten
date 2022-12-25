#include "renderer.hpp"
#include "core/scene.hpp"
#include "components/components.hpp"
#include "subsystems/render/skybox-manager.hpp"
namespace engine::render
{
    void Renderer::Render(core::Scene *scene, render::PerFrame const & per_frame)
    {
        opaque_render_system_.Render(scene);
        emissive_render_system_.Render();
        for (auto entity : scene->registry.view<components::SkyboxComponent>())
        {
            SkyboxManager::RenderSkybox(scene->registry.get<components::SkyboxComponent>(entity), per_frame);
        }
        particle_render_system_.Render(scene);
    }
    void Renderer::Tick(core::Scene *scene, float delta_time)
    {
        particle_render_system_.Tick(scene, delta_time);
    }
    void Renderer::Update(core::Scene *scene)
    {
        light_render_system_.Update(scene);
    }
    void Renderer::OnInstancesUpdated(core::Scene *scene)
    {
        opaque_render_system_.OnInstancesUpdated(scene->registry);
        emissive_render_system_.OnInstancesUpdated(scene->registry);
        light_render_system_.OnInstancesUpdated(scene);
    }
}