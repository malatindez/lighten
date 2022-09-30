#include "renderer.hpp"
using namespace engine;
using namespace core;
using namespace direct3d;
using namespace math;
using namespace ShaderCompiler;
void Renderer::OnGuiRender()
{
}
Renderer::Renderer(ivec2 const &screen_resolution, engine::render::PerFrame &per_frame) : per_frame(per_frame), screen_resolution(screen_resolution)
{
}
void Renderer::OnRender()
{
    auto const &camera = Engine::scene()->main_camera->camera();
    per_frame.view = camera.view;
    per_frame.projection = camera.projection;
    per_frame.view_projection = camera.view_projection;
    per_frame.inv_view = camera.inv_view;
    per_frame.inv_projection = camera.inv_projection;
    per_frame.inv_view_projection = camera.inv_view_projection;
    per_frame.screen_resolution = vec2{ screen_resolution };
    per_frame.mouse_position = vec2{ InputLayer::instance()->mouse_position() };
    render::ModelSystem::instance().Render(Engine::scene()->registry);

    for (auto entity : Engine::scene()->registry.view<components::SkyboxComponent>())
    {
        SkyboxManager::RenderSkybox(Engine::scene()->registry.get<components::SkyboxComponent>(entity), per_frame);
    }
}