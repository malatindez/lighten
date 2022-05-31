#include "pch.hpp"

#include "controller.hpp"
#include "core/application.hpp"
#include "misc/camera-controller.hpp"
#include "components/plane.hpp"
#include "components/material.hpp"
#include "components/mesh.hpp"
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
    registry.emplace<Material>(plane, math::vec3{0.1f});
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

    
    auto scene = std::make_shared<Scene>();
    entt::registry& registry = scene->registry;

    entt::entity sphere = registry.create();
    AddBasicSphere(registry, sphere);
    registry.get<Transform>(sphere).position = math::vec3{ 0, -1, -2 };
    registry.get<Transform>(sphere).UpdateMatrices();
    registry.get<Material>(sphere).color = Color{1.0f, 1.0f, 0.0f};
        
    entt::entity sphere2 = registry.create();
    AddBasicSphere(registry, sphere2);
    registry.get<Transform>(sphere2).position = math::vec3{ 0, 1, -2 };
    registry.get<Transform>(sphere2).UpdateMatrices();
    registry.get<Material>(sphere2).color = Color{1.0f, 0.0f, 1.0f};

    entt::entity plane = registry.create();
    AddBasicPlane(registry, plane);
    registry.get<Plane>(plane).update_plane(math::vec3{ 0,0,1 }, math::vec3{ 1,0,0 });
    registry.get<Transform>(plane).position = math::vec3{ 0, -2, 0 };
    registry.get<Transform>(plane).UpdateMatrices();

    entt::entity point_light_entity = registry.create();
    AddBasicSphere(registry, point_light_entity);
    registry.get<Transform>(point_light_entity).position = math::vec3{ 2, 2, -1 };
    registry.get<Transform>(point_light_entity).scale = math::vec3{0.01f};
    registry.get<Transform>(point_light_entity).UpdateMatrices();

    PointLight &point_light = registry.emplace<PointLight>(point_light_entity);
    point_light.color = math::vec3{ 0.9f,0.0f,0.5f };
    point_light.ambient_intensity = 0.2f;
    point_light.diffuse_intensity = 0.6f;
    point_light.specular_intensity = 2.5f;
    point_light.attenuation = PointLight::Attenuation
    {
        .constant = 1.0f,
        .linear = 1.7f,
        .quadratic = 1.44f
    };
    
    entt::entity directional_light = registry.create();

    DirectionalLight& directional_light_v = registry.emplace<DirectionalLight>(directional_light);
    directional_light_v.direction = math::normalize(math::vec3{ 0,-1, 0 });
    directional_light_v.color = math::vec3{ 1,1,0.7f };
    directional_light_v.ambient_intensity = 0.5f;
    directional_light_v.diffuse_intensity = 0.5f;
    directional_light_v.specular_intensity = 0.5f;

    

    entt::entity camera = registry.create();
    Transform &camera_transform = registry.emplace<Transform>(camera);
    camera_transform.position = math::vec3{0,0,-5};
    camera_transform.UpdateMatrices();
    Camera &cam = registry.emplace<Camera>(camera);
    CameraController camera_controller(cam, camera_transform, bmwindow->window_size());

    
    entt::entity cube = registry.create();
    Transform &cube_transform = registry.emplace<Transform>(cube);
    cube_transform.position = math::vec3{2,2,0};
    cube_transform.UpdateMatrices();
    registry.emplace<Cube>(cube);
    registry.emplace<Material>(cube, math::vec3{1.0f, 1.0f, 0.0f});
    
    entt::entity mesh = registry.create();
    Transform &mesh_transform = registry.emplace<Transform>(mesh);
    mesh_transform.position = math::vec3{2,4,0};
    mesh_transform.UpdateMatrices();
    registry.emplace<Mesh>(mesh, render::LoadMeshFromObj(std::filesystem::current_path() / "objects/cube.obj"));
    registry.emplace<Material>(mesh, math::vec3{1.0f, 1.0f, 0.0f});

    Application::Init();

    bmwindow->SetEventCallback(Application::event_function());
    

    auto controller = std::make_shared<Controller>(*bmwindow, scene, camera_controller);

    class LightUpdater : public engine::Layer
    {
    public:
        LightUpdater(DirectionalLight &dir_light) : dir_light(dir_light) {}

        void OnEvent(engine::Event& event) override
        {
            if (event.type() == EventType::AppTick)
            {
                AppTickEvent const&ate = dynamic_cast<AppTickEvent&>(event);
                t += ate.delta_time();
                dir_light.direction = math::vec3
                {
                    math::sin(t),
                    math::cos(t),
                    0
                };
            }
        }
        float t = 0;
        DirectionalLight& dir_light;
    };

    auto lupdater = std::make_shared<LightUpdater>(directional_light_v);

    Application &application = Application::Get();
    application.AddLayer(controller);
    application.AddLayer(lupdater);
    application.Run();
    PostQuitMessage(0);
    return 0;
}