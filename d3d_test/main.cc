#include "controller.hpp"
#include "core/application.hpp"
#include "direct3d11/common.hpp"
#include "pch.hpp"
#include "renderer.hpp"
#include <filesystem>
#include "platform/windows/render-pipeline.hpp"
using namespace engine;
using namespace core;
using namespace math;
using namespace platform;

// the entry point for any Windows program
INT WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    Application::Init();

    auto render_pipeline = std::make_shared<windows::RenderPipeline>();
    render_pipeline->PushLayer(std::make_shared<Renderer>());

    Application &app = Application::Get();

    app.PushLayer(std::make_shared<Controller>());
    app.PushLayer(render_pipeline);

    app.Run();
    return 0;
}
