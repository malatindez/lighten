#include "controller.hpp"
#include "core/engine.hpp"
#include "core/layers/input-layer.hpp"
#include "core/layers/model-loader.hpp"
#include "core/layers/shader-manager.hpp"
#include "platform/windows/windows-render-pipeline.hpp"
#include "render/uniform-buffer.hpp"
#include "renderer.hpp"
using namespace engine;
using namespace core;
using namespace math;
using namespace platform;

// the entry point for any Windows program
INT WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    Engine::Init();
    Engine &app = Engine::Get();

    try
    {
        // Initialize in-engine layers that we need
        auto shader_manager = ShaderManager::instance();
        auto input_layer = InputLayer::instance();
        app.PushLayer(shader_manager);
        app.PushLayer(input_layer);

        auto render_pipeline = std::make_shared<windows::RenderPipeline>();
        auto renderer = std::make_shared<Renderer>();

        render_pipeline->PushLayer(renderer);
        auto controller = std::make_shared<Controller>(renderer, render_pipeline->window()->size());
        render_pipeline->PushLayer(controller);
        render_pipeline->window()->SetEventCallback(Engine::event_function());

        app.PushLayer(render_pipeline);

        shader_manager = nullptr;
        render_pipeline = nullptr;

        app.Run();
    }
    catch (...)
    {
        spdlog::critical("Exception occurred within the engine. Shutting down.");
        app.Exit();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    Engine::Deinit();
    return 0;
}
