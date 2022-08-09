#include "controller.hpp"
#include "core/application.hpp"
#include "core/swapchain-window.hpp"
#include "direct3d/common.hpp"
#include "pch.hpp"
#include "renderer.hpp"
#include <filesystem>
using namespace engine;
using namespace core;
using namespace math;

// the entry point for any Windows program
INT WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int cmd_show)
{
    Application::Init();

    WNDCLASSEXW wc;
    // clear out the window class for use
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    // fill in the struct with the needed information
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(nullptr, IDC_CROSS);
    wc.hbrBackground = (HBRUSH) COLOR_WINDOW;

    // create the window and use the result as the handle
    auto window = std::make_shared<SwapchainWindow>(
        wc, NULL,
        L"WindowClass1",                  // name of the window class
        L"Engine",                        // title of the window
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, // window style
        kWindowPosition,                  // position of the window
        kWindowResolution,                // window size
        nullptr,                          // we have no parent window, nullptr
        nullptr,                          // we aren't using menus, nullptr
        instance,                         // application handle
        nullptr);                         // used with multiple windows, nullptr

    // display the window on the screen
    ShowWindow(window->handle(), cmd_show);

    window->SetEventCallback(Application::event_function());

    auto controller = std::make_shared<Controller>(std::static_pointer_cast<Window>(window));

    auto renderer = std::make_shared<Renderer>(window);

    Application::Get().AddLayer(controller);
    Application::Get().AddLayer(renderer);

    Application::Get().Run();
    return 0;
}
