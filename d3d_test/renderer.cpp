#include "renderer.hpp"
#include "misc/skybox-manager.hpp"
using namespace engine;
using namespace core;
using namespace direct3d;
using namespace math;
using namespace ShaderCompiler;
Renderer::Renderer()
{
    knight_model_id = ModelLoader::instance()->Load("assets\\models\\Knight\\Knight.fbx").value_or(0);
}
void Renderer::OnRender()
{
    per_frame.view_projection = Engine::scene()->main_camera->camera().view_projection;
    per_frame_buffer.Update(per_frame);
    per_frame_buffer.Bind(direct3d::ShaderType::VertexShader, 0);
    for (auto entity : Engine::scene()->registry.view<components::SkyboxComponent>())
    {
        SkyboxManager::RenderSkybox(Engine::scene()->registry.get<components::SkyboxComponent>(entity), Engine::scene()->main_camera->transform().position);
    }
    render::ModelSystem::instance()->Render();
}