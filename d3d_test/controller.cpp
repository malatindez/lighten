#include "controller.hpp"
#include "transform-editor.hpp"
#include "camera-movement.hpp"
using namespace engine;
using namespace core;
using namespace events;
using namespace math;
using namespace components;

void Controller::OnGuiRender()
{
    ImGui::Begin("Framerate");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::Begin("Camera info");
    ImGui::Text("Move speed: ");
    ImGui::Text("%.3f", Engine::scene()->main_camera->move_speed());
    ImGui::Text("FOV: ");
    ImGui::Text("%.3f", Engine::scene()->main_camera->fovy());
    ImGui::Text("Flags: ");
    ImGui::Text("%u", Engine::scene()->main_camera->flags());
    ImGui::Text("Camera model matrix");
    ImGui::Text("%s", utils::FormatToString(Engine::scene()->main_camera->transform().model).c_str());
    ImGui::Text("Camera inv view matrix");
    ImGui::Text("%s", utils::FormatToString(Engine::scene()->main_camera->camera().inv_view).c_str());
    ImGui::End();
    static bool window = true;
    ImGui::ShowDemoWindow(&window);
    ImGui::Begin("Transform edit");
    transform_editor::OnGuiRender(window_pos, window_size);
    ImGui::End();
}

Controller::Controller(std::shared_ptr<Renderer> renderer, math::ivec2 const &window_size, math::ivec2 const &window_pos) : renderer_{ renderer }, window_size{ window_size }, window_pos{ window_pos }
{
    first_scene = std::make_shared<Scene>();

    auto &registry = first_scene->registry;
    main_camera_entity = registry.create();
    registry.emplace<CameraComponent>(main_camera_entity, CameraComponent());
    registry.emplace<TransformComponent>(main_camera_entity, TransformComponent());
    first_scene->main_camera = std::make_unique<CameraController>(&registry, main_camera_entity, window_size);
    Engine::SetScene(first_scene);
    int amount = 10;
    for (int i = 0; i < amount; i++)
    {
        auto knight = registry.create();
        last_created_knight = knight;
        auto &transform = registry.emplace<TransformComponent>(knight);

        auto model_id = ModelLoader::Load("assets\\models\\Knight\\Knight.fbx").value();
        render::ModelSystem::instance().AddOpaqueInstance(model_id, registry, knight);
        transform.position = vec3
        {
            std::sin(float(i) / amount * 2 * (float)std::numbers::pi),
            0,
            std::cos(float(i) / amount * 2 * (float)std::numbers::pi)
        } *(float(amount) / std::sqrtf((float)amount));
        transform.rotation = QuaternionFromEuler(0.0f, 0.0f, radians(180.0f));
        transform.rotation *= QuaternionFromRotationMatrix(look_at(transform.position, vec3{ 0,0,0 }, vec3{ 0,1,0 }).as_rmat<3, 3>());
        transform.UpdateMatrices();
    }
    /*
    const auto skybox_path = std::filesystem::current_path() / "assets/textures/skyboxes/yokohama";
    SkyboxManager::LoadSkybox(registry, std::array<std::filesystem::path, 6> {
        skybox_path / "posx.jpg",
            skybox_path / "negx.jpg",
            skybox_path / "posy.jpg",
            skybox_path / "negy.jpg",
            skybox_path / "posz.jpg",
            skybox_path / "negz.jpg",
    });*/
    SkyboxManager::LoadSkybox(registry, std::filesystem::current_path() / "assets/textures/skyboxes/skybox.dds");
    {
        auto cube = registry.create();
        registry.emplace<TestCubeComponent>(cube);
        auto &cube_transform = registry.emplace<TransformComponent>(cube);
        cube_transform.reset();
        cube_transform.position = vec3{ 0,3,0 };
        cube_transform.scale = vec3{ 0.01f };
        cube_transform.rotation = QuaternionFromEuler(0.0f, 0.0f, 0.0f);
        cube_transform.UpdateMatrices();
        auto model_id = ModelLoader::Load("assets\\models\\Cube\\Cube.fbx").value();
        render::ModelSystem::instance().AddOpaqueInstance(model_id, registry, cube);
    }
    { // Add two opaque cubes with two different materials
        auto model_id = ModelLoader::Load("assets\\models\\Cube\\Cube.fbx").value();
        {
            auto cube = registry.create();
            render::ModelSystem::instance().AddOpaqueInstance(model_id, registry, cube);
            auto &cube_transform = registry.emplace<TransformComponent>(cube);
            cube_transform.reset();
            cube_transform.position = vec3{ -10,1,0 };
            cube_transform.scale = vec3{ 0.01f };
            cube_transform.rotation = QuaternionFromEuler(0.0f, 0.0f, 0.0f);
            cube_transform.UpdateMatrices();
        }
        {
            auto cube = registry.create();
            auto texture_view = TextureManager::GetTextureView(TextureManager::LoadTexture(std::filesystem::current_path() / "assets\\models\\Cube\\textures\\cube2.jpg"));
            render::ModelSystem::instance().AddOpaqueInstance(model_id, registry, cube, { render::OpaqueMaterial(texture_view) });
            auto &cube_transform = registry.emplace<TransformComponent>(cube);
            cube_transform.reset();
            cube_transform.position = vec3{ -10,3,0 };
            cube_transform.scale = vec3{ 0.01f };
            cube_transform.rotation = QuaternionFromEuler(0.0f, 0.0f, 0.0f);
            cube_transform.UpdateMatrices();
        }
    }
    render::ModelSystem::instance().OnInstancesUpdated(registry);
    camera_movement::RegisterKeyCallbacks();
    transform_editor::RegisterKeyCallbacks();
}
void Controller::OnTick([[maybe_unused]] float delta_time)
{
    for (auto const &func : update_callbacks_)
    {
        func(delta_time);
    }
    auto &input = *InputLayer::instance();
    auto scene = Engine::scene();
    camera_movement::UpdateCamera(delta_time);

    renderer_->per_frame.view_projection = scene->main_camera->camera().view_projection;

    if (input.mouse_scrolled())
    {
        auto &move_speed = scene->main_camera->move_speed();
        move_speed = std::max(0.01f, move_speed * (input.scroll_delta() > 0 ? 1.1f : 1.0f / 1.1f));
        input.flush();
    }
}
void Controller::OnEvent(engine::core::events::Event &e)
{
    if (e.type() == engine::core::events::EventType::WindowClose) [[unlikely]]
    {
        engine::core::Engine::Exit();
    }
    else if (e.type() == engine::core::events::EventType::WindowResize) [[unlikely]]
    {
        Engine::scene()->main_camera->UpdateProjectionMatrix();
    }
}