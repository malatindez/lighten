#include "controller.hpp"
#include "core/engine.hpp"
#include "platform/windows/windows-render-pipeline.hpp"
#include "renderer.hpp"
#include "render/uniform-buffer.hpp"
#include "core/layers/shader-manager.hpp"
#include "core/layers/model-loader.hpp"
#include "core/layers/input-layer.hpp"
using namespace engine;
using namespace core;
using namespace math;
using namespace platform;

// the entry point for any Windows program
INT WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    Engine::Init();
    Engine &app = Engine::Get();

    // Initialize in-engine layers that we need
    auto shader_manager = ShaderManager::instance();
    auto input_layer = InputLayer::instance();
    app.PushLayer(shader_manager);
    app.PushLayer(input_layer);


    auto render_pipeline = std::make_shared<windows::RenderPipeline>();
    auto renderer = std::make_shared<Renderer>();
    render_pipeline->PushLayer(renderer);
    render_pipeline->window()->SetEventCallback(Engine::event_function());

    auto controller = std::make_shared<Controller>(renderer, render_pipeline->window()->size());



    app.PushLayer(controller);
    app.PushLayer(render_pipeline);

    shader_manager = nullptr;
    render_pipeline = nullptr;

    app.Run();

    Engine::Deinit();
    return 0;
}
