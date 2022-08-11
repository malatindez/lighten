#include "controller.hpp"
#include "core/engine.hpp"
#include "platform/windows/windows-render-pipeline.hpp"
#include "renderer.hpp"
#include "render/uniform-buffer.hpp"
#include "core/layers/shader-manager.hpp"
using namespace engine;
using namespace core;
using namespace math;
using namespace platform;

// the entry point for any Windows program
INT WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    Engine::Init();

    auto shader_manager = ShaderManager::instance();

    auto render_pipeline = std::make_shared<windows::RenderPipeline>();
    render_pipeline->PushLayer(std::make_shared<Renderer>(shader_manager));
    render_pipeline->window()->SetEventCallback(Engine::event_function());

    Engine &app = Engine::Get();
    app.PushLayer(shader_manager);
    app.PushLayer(std::make_shared<Controller>());
    app.PushLayer(render_pipeline);

    app.Run();

    Engine::Deinit();
    return 0;
}
