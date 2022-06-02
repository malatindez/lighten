#include "pch.hpp"

#include "components/material.hpp"
#include "components/mesh.hpp"
#include "components/plane.hpp"
#include "controller.hpp"
#include "core/application.hpp"
#include "misc/camera-controller.hpp"
#include "tick-layer.hpp"
using namespace engine;
using namespace components;

void AddBasicSphere(entt::registry &registry, entt::entity sphere)
{
    registry.emplace<Material>(sphere, math::vec3{1.0f, 0.1f, 0.1f});
    registry.emplace<Sphere>(sphere);
    registry.emplace<Transform>(sphere).reset();
}

void AddBasicPlane(entt::registry &registry, entt::entity plane)
{
    registry.emplace<Material>(plane, math::vec3{0.3f});
    registry.emplace<Plane>(plane);
    registry.emplace<Transform>(plane).reset();
}
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

    auto tick_layer = std::make_shared<OnTickLayer>();

    auto scene = std::make_shared<Scene>();
    entt::registry &registry = scene->registry;

    entt::entity sphere = registry.create();
    AddBasicSphere(registry, sphere);
    registry.get<Transform>(sphere).position = math::vec3{0, 1, -2};
    registry.get<Transform>(sphere).UpdateMatrices();
    registry.get<Material>(sphere).albedo = Color{1.0f, 1.0f, 1.0f};
    entt::entity plane = registry.create();
    AddBasicPlane(registry, plane);
    registry.get<Plane>(plane).update_plane(math::vec3{0, 0, 1}, math::vec3{1, 0, 0});
    registry.get<Transform>(plane).position = math::vec3{0, -2, 0};
    registry.get<Transform>(plane).UpdateMatrices();

    entt::entity point_light_entity = registry.create();
    AddBasicSphere(registry, point_light_entity);
    Transform &plet = registry.get<Transform>(point_light_entity);
    plet.position = math::vec3{2, 2, -1};
    plet.scale = math::vec3{0.001f};
    plet.UpdateMatrices();

    PointLight &point_light = registry.emplace<PointLight>(point_light_entity);
    point_light.color = math::vec3{0.9f, 0.5f, 0.5f};
    point_light.R = 5.00f;
    registry.get<Material>(point_light_entity).albedo = math::vec3{0};
    registry.get<Material>(point_light_entity).emission = point_light.color;

    entt::entity moving_sphere = registry.create();
    AddBasicSphere(registry, moving_sphere);
    auto &s2t = registry.get<Transform>(moving_sphere);
    s2t.position = math::vec3{0, 1, -2};
    s2t.scale = math::vec3{0.5f};
    s2t.UpdateMatrices();
    float s2t_t = 0;
    tick_layer->funcs.emplace_back(
        [&s2t, &s2t_t, &plet](float dt)
        {
            s2t_t += dt;
            s2t.position = plet.position + math::vec3{sin(s2t_t), -1.0f, math::cos(s2t_t)};
            s2t.scale = math::vec3{0.25f + math::cos(s2t_t / 4) / 8.0f};
            s2t.UpdateMatrices();
        });
    registry.get<Material>(moving_sphere).albedo = Color{1.0f, 0.0f, 1.0f};

    entt::entity directional_light = registry.create();

    DirectionalLight &directional_light_v = registry.emplace<DirectionalLight>(directional_light);
    directional_light_v.direction = math::normalize(math::vec3{0, -1, -1});
    directional_light_v.color = math::vec3{0.3f, 0.0f, 0.4f};
    float dir_light_t = 0.0f;
    /*     tick_layer->funcs.push_back(
            [&dir_light_t, &directional_light_v](float delta_time)
            {
                dir_light_t += delta_time;
                directional_light_v.direction = math::normalize(math::vec3{
                    math::sin(dir_light_t),
                    math::cos(dir_light_t),
                    0 });
            });   */

    entt::entity camera = registry.create();
    Transform &camera_transform = registry.emplace<Transform>(camera);
    camera_transform.position = math::vec3{0, 0, -5};
    camera_transform.UpdateMatrices();
    Camera &cam = registry.emplace<Camera>(camera);
    CameraController camera_controller(cam, camera_transform, bmwindow->window_size());

    SpotLight &spot = registry.emplace<SpotLight>(camera);
    spot.direction = math::normalize(math::vec3{0, -1, 0});
    spot.color = math::vec3{0.3f, 0.3f, 0.7f};
    spot.cut_off = math::radians(45.0F);
    spot.R = 1.0f;
    float t_temp = 0;

    entt::entity cube = registry.create();
    Transform& cube_transform = registry.emplace<Transform>(cube);
    cube_transform.position = math::vec3{ 0, 0, 0 };
    cube_transform.UpdateMatrices();
    registry.emplace<Cube>(cube);
    registry.emplace<Material>(cube, math::vec3{ 1.0f, 1.0f, 0.0f });

    entt::entity cube2 = registry.create();
    Transform& cube2_transform = registry.emplace<Transform>(cube2);
    cube2_transform.position = math::vec3{ 0, 2, 2 };
    cube2_transform.UpdateMatrices();
    registry.emplace<Cube>(cube2);
    registry.emplace<Material>(cube2, math::vec3{ 1.0f, 1.0f, 0.0f });

    /*  entt::entity mesh = registry.create();
     Transform &mesh_transform = registry.emplace<Transform>(mesh);
     mesh_transform.position = math::vec3{2,4,0};
    mesh_transform.UpdateMatrices();
     registry.emplace<Mesh>(mesh, render::LoadMeshFromObj(std::filesystem::current_path() / "objects/cube.obj"));
    registry.emplace<Material>(mesh, math::vec3{1.0f, 1.0f, 0.0f}); */

    Application::Init();

    bmwindow->SetEventCallback(Application::event_function());

    auto controller = std::make_shared<Controller>(*bmwindow, scene, camera_controller);

    tick_layer->funcs.emplace_back(
        [&spot, &controller](float)
        {
            spot.direction = controller->camera_controller().forward();
        });
    scene->OnRegistryUpdate();
    Application &application = Application::Get();
    application.AddLayer(controller);
    application.AddLayer(tick_layer);
    application.Run();
    PostQuitMessage(0);
    return 0;
}