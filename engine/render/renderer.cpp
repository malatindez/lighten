#include "renderer.hpp"
#include "core/scene.hpp"
namespace engine::render
{

    void Renderer::Render(core::Scene *scene)
    {

        opaque_render_system_.Render(scene);
        emissive_render_system_.Render();
    }

    void Renderer::OnInstancesUpdated(core::Scene *scene)
    {

        opaque_render_system_.OnInstancesUpdated(scene->registry);
        emissive_render_system_.OnInstancesUpdated(scene->registry);
        light_render_system_.OnInstancesUpdated(scene);

    }
}