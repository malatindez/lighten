#include "controller.hpp"
#include "platform/windows/windows-hdr-render-pipeline.hpp"
using namespace engine;
using namespace core;
using namespace math;
using namespace platform;

// the entry point for any Windows program
INT WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    Engine::Init();
    Engine &app = Engine::Get();
    spdlog::info(std::to_string(sizeof(render::Material)));
    try
    {
        // Initialize in-engine layers that we need
        auto shader_manager = ShaderManager::instance();
        auto input_layer = InputLayer::instance();
        app.PushLayer(shader_manager);
        app.PushLayer(input_layer);

        auto render_pipeline = std::make_shared<windows::HDRRenderPipeline>();
        auto renderer = std::make_shared<Renderer>(render_pipeline->window()->size(), render_pipeline->per_frame());

        render_pipeline->PushLayer(renderer);
        auto controller = std::make_shared<Controller>(renderer, render_pipeline);
        render_pipeline->PushLayer(controller);
        render_pipeline->window()->SetEventCallback(Engine::event_function());

        app.PushLayer(render_pipeline);

        shader_manager = nullptr;
        render_pipeline = nullptr;
        renderer = nullptr;

        app.Run();
    }
    catch (...)
    {
        spdlog::critical("Exception occurred within the engine. Shutting down.");
        app.Exit();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    Engine::Deinit();
    std::this_thread::sleep_for(std::chrono::milliseconds(750));
    return 0;
}