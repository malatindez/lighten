#include "controller.hpp"
#include "direct3d11/hdr-render-pipeline.hpp"
#include "platform/windows/windows-window.hpp"
#include "render/present-swapchain.hpp"
#include "core/engine.hpp"
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
        app.PushLayer(input_layer);
        app.PushLayer(shader_manager);

        std::shared_ptr<windows::Window> window = std::make_shared<windows::Window>(core::Window::Props{ "D3D Test", { 1280, 720 }, {100, 100} });
        window->SetEventCallback(Engine::event_function());
        app.PushLayer(window);

        auto swapchain_render_target = std::make_shared<direct3d::SwapchainRenderTarget>();
        swapchain_render_target->init(window->handle(), window->size());

        auto render_pipeline = std::make_shared<direct3d::HDRRenderPipeline>(std::static_pointer_cast<core::Window>(window), swapchain_render_target);
        render_pipeline->SetOutputTarget(swapchain_render_target);
        render_pipeline->WindowSizeChanged(window->size());

        std::shared_ptr<render::PresentSwapchain> present_swapchain = std::make_shared<render::PresentSwapchain>();
        render_pipeline->post_processing().AddLayer(present_swapchain);

        auto controller = std::make_shared<Controller>(window->size(), window->position(), render_pipeline->hdr_to_ldr_layer()->exposure());
        render_pipeline->PushLayer(controller);
        render_pipeline->SetScene(Engine::scene());
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
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    Engine::Deinit();
    std::this_thread::sleep_for(std::chrono::milliseconds(750));
    return 0;
}