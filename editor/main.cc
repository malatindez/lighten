#include "controller.hpp"
#include "direct3d11/deferred-hdr-render-pipeline.hpp"
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
#ifndef _DEBUG
    try
    {
#endif
        // Initialize in-engine layers that we need
        auto shader_manager = ShaderManager::instance();
        auto input_layer = InputLayer::instance();
        app.PushLayer(shader_manager);

        std::shared_ptr<windows::Window> window = std::make_shared<windows::Window>(core::Window::Props{"D3D Test", {1280, 720}, {100, 100}});
        window->SetEventCallback(Engine::event_function());
        app.PushLayer(window);

        auto swapchain_render_target = std::make_shared<direct3d::SwapchainRenderTarget>();
        swapchain_render_target->init(window->handle(), window->size());

        auto render_pipeline = std::make_shared<direct3d::DeferredHDRRenderPipeline>(std::static_pointer_cast<core::Window>(window), swapchain_render_target);
        render_pipeline->WindowSizeChanged(window->size());

        auto controller = std::make_shared<Controller>(render_pipeline);
        render_pipeline->PushLayer(controller);
        render_pipeline->SetScene(Engine::scene());
        app.PushLayer(render_pipeline);

        shader_manager = nullptr;
        render_pipeline = nullptr;
        app.PushLayer(input_layer);
        app.Run();
#ifndef _DEBUG
    }
    catch (std::exception e)
    {
        spdlog::critical(e.what());
        spdlog::critical("Exception occurred within the engine. Shutting down.");
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        app.Exit();
    }
    catch (...)
    {
        spdlog::critical("Unknown exception occurred within the engine. Shutting down.");
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        app.Exit();
    }
#endif
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    Engine::Deinit();
    std::this_thread::sleep_for(std::chrono::milliseconds(750));
    return 0;
}