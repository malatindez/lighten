#include "pch.hpp"

#include "controller.hpp"
#include "core/application.hpp"
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
    // display the window on the screen
    ShowWindow(bmwindow->handle(), cmd_show);

    auto scene = std::make_shared<Scene>();

    scene->sphere = math::Sphere{math::vec3{0, -1, -2}, 0.5f};

    engine::components::PointLight point_light;
    point_light.position = math::vec3{ 0,0.25f,0 };
    point_light.color = engine::math::vec3{ 0.5f,0.0f,0.5f };
    point_light.ambient_intensity = 0.2f;
    point_light.diffuse_intensity = 0.3f;
    point_light.specular_intensity = 0.1f;
    point_light.attenuation = engine::components::PointLight::Attenuation
    {
        .constant = 1.0f,
        .linear = 0.7f,
        .quadratic = 0.44f
    };

    engine::components::DirectionalLight directional_light;
    directional_light.direction = math::normalize(math::vec3{ 0,-1, 0 });
    directional_light.color = engine::math::vec3{ 1,1,0.7f };
    directional_light.ambient_intensity = 0.2f;
    directional_light.diffuse_intensity = 0.3f;
    directional_light.specular_intensity = 0.1f;

    scene->point_lights.push_back( point_light );
    scene->directional_lights.push_back(directional_light);
    auto controller = std::make_shared<Controller>(*bmwindow, scene);
    
    Application::Init();

    bmwindow->SetEventCallback(Application::event_function());
    

    Application &application = Application::Get();
    application.AddLayer(controller);
    application.Run();
    PostQuitMessage(0);

    return 0;
}