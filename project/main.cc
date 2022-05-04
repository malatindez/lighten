#include <Windows.h>

#include "controller.hpp"
#include "engine.hpp"
using namespace engine;

// the entry point for any Windows program
INT WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, PWSTR cmd_line,
                    int cmd_show)
{
    WNDCLASSEXW wc;

    // clear out the window class for use
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    // fill in the struct with the needed information
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(nullptr, IDC_CROSS);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

    // create the window and use the result as the handle
    auto bmwindow = std::make_shared<BitmapWindow>(
        wc, NULL,
        L"WindowClass1",                  // name of the window class
        L"Our First Windowed Program",    // title of the window
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, // window style
        kWindowPosition,                  // position of the window
        kWindowResolution,                // window size
        nullptr,                          // we have no parent window, nullptr
        nullptr,                          // we aren't using menus, nullptr
        instance,                         // application handle
        nullptr);                         // used with multiple windows, nullptr
    auto window = std::static_pointer_cast<Window>(bmwindow);

    // display the window on the screen
    ShowWindow(window->handle(), cmd_show);

    auto scene = std::make_shared<Scene>();

    scene->sphere = math::Sphere{{0, 0, -1}, 0.5f};

    auto controller = std::make_shared<Controller>(*bmwindow, scene);

    Engine &engine = GetEngine();
    engine.AddWindow(window);
    engine.AddUpdatable(controller);
    engine.Start();
    PostQuitMessage(0);
    return 0;
}