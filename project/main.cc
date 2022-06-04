#include "pch.hpp"
#define NOMINMAX
#include <Windows.h>

#include "render/material.hpp"
#include "components/mesh-component.hpp"
#include "controller.hpp"
#include "core/application.hpp"
#include "misc/camera-controller.hpp"
using namespace engine;
using namespace components;

void AddBasicSphere(entt::registry &registry, entt::entity sphere)
{
}

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
        L"Our First Windowed Program",    // title of the window
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, // window style
        kWindowPosition,                  // position of the window
        kWindowResolution,                // window size
        nullptr,                          // we have no parent window, nullptr
        nullptr,                          // we aren't using menus, nullptr
        instance,                         // application handle
        nullptr);                         // used with multiple windows, nullptr
    // display the window on the screen
    ShowWindow(bmwindow->handle(), cmd_show);

    auto scene = std::make_shared<Scene>();
    entt::registry &registry = scene->registry;




    entt::entity camera = registry.create();
    Transform &camera_transform = registry.emplace<Transform>(camera);
    camera_transform.position = math::vec3{0, 0, -10};
    camera_transform.UpdateMatrices();
    Camera &cam = registry.emplace<Camera>(camera);
    CameraController camera_controller(cam, camera_transform, bmwindow->window_size());

    Application::Init();

    bmwindow->SetEventCallback(Application::event_function());

    auto controller = std::make_shared<Controller>(*bmwindow, scene, camera_controller);

    controller->InitScene();

    SpotLight &spot = registry.emplace<SpotLight>(camera);
    spot.direction = math::normalize(math::vec3{0, -1, 0});
    spot.color = math::vec3{0.3f, 0.3f, 0.7f};
    spot.cut_off = math::radians(45.0F);
    spot.R = 1.0f;

    controller->update_callbacks().emplace_back(
        [&spot, &controller](float)
        {
            spot.direction = controller->camera_controller().forward();
        });
    Application &application = Application::Get();
    application.AddLayer(controller);
    application.Run();
    PostQuitMessage(0);
    return 0;
}