#include "pch.hpp"
#define NOMINMAX
#include <Windows.h>

#include "render/material.hpp"
#include "components/scene-mesh.hpp"
#include "controller.hpp"
#include "core/application.hpp"
#include "misc/camera-controller.hpp"
using namespace engine;
using namespace core;
using namespace math;
using namespace components;

// the entry point for any Windows program
INT WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int cmd_show)
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
        L"Engine",                        // title of the window
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, // window style
        kWindowPosition,                  // position of the window
        kWindowResolution,                // window size
        nullptr,                          // we have no parent window, nullptr
        nullptr,                          // we aren't using menus, nullptr
        instance,                         // application handle
        nullptr);                         // used with multiple windows, nullptr
    // display the window on the screen
    ShowWindow(bmwindow->handle(), cmd_show);


    Application::Init();

    bmwindow->SetEventCallback(Application::event_function());

    auto controller = std::make_shared<Controller>(*bmwindow);

    Application &application = Application::Get();
    application.AddLayer(controller);
    application.Run();
    PostQuitMessage(0);
    return 0;
}