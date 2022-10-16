#include "controller.hpp"
#include "camera-movement.hpp"
#include "transform-editor.hpp"
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
    ImGui::SliderFloat("Exposure", &exposure_, -20.0f, 20.0f);
    ImGui::SliderFloat("Default AO", &render::ModelSystem::instance().opaque_render_system().ambient_occlusion(), 0.0f, 1.0f);

    ImGui::End();
    object_editor::OnGuiRender(window_pos, window_size);
}

Controller::Controller(std::shared_ptr<Renderer> renderer, math::ivec2 const &window_size, math::ivec2 const &window_pos, float &exposure)
    : exposure_{ exposure }, window_size{ window_size }, window_pos{ window_pos }, renderer_{ renderer }
{
    first_scene = std::make_shared<Scene>();
    auto &ors = render::ModelSystem::instance().opaque_render_system();
    ors.SetBrdfTexture(TextureManager::GetTextureView(std::filesystem::current_path() / "assets/textures/IBL/ibl_brdf_lut.dds"));
    ors.SetIrradianceTexture(TextureManager::GetTextureView(std::filesystem::current_path() / "assets/textures/skyboxes/night_street/night_street_irradiance.dds"));
    ors.SetPrefilteredTexture(TextureManager::GetTextureView(std::filesystem::current_path() / "assets/textures/skyboxes/night_street/night_street_reflection.dds"));
    auto &registry = first_scene->registry;
    main_camera_entity = registry.create();
    registry.emplace<CameraComponent>(main_camera_entity, CameraComponent());
    registry.emplace<TransformComponent>(main_camera_entity, TransformComponent());
    first_scene->main_camera = std::make_unique<CameraController>(&registry, main_camera_entity, window_size);
    Engine::SetScene(first_scene);
    Engine::scene()->main_camera->SetWorldOffset(core::math::vec3{ 0.0f, 0.0f, 0.0f });
    Engine::scene()->main_camera->SetWorldAngles(0.0f, 0.0f, 0.0f);
    Engine::scene()->main_camera->SetProjectionMatrix(perspective(45.0f, float(window_size.x) / float(window_size.y), 0.1f, 100.0f));

    int amount = 10;
    for (int i = 0; i < amount; i++)
    {
        auto knight = registry.create();
        last_created_knight = knight;
        auto &transform = registry.emplace<TransformComponent>(knight);
        uint64_t model_id = ModelLoader::Load("assets\\models\\Knight\\Knight.fbx").value();
        if (i % 4 == 1) model_id = ModelLoader::Load("assets\\models\\Samurai\\Samurai.fbx").value();
        else if (i % 4 == 2) model_id = ModelLoader::Load("assets\\models\\KnightHorse\\KnightHorse.fbx").value();
        else if (i % 4 == 3) model_id = ModelLoader::Load("assets\\models\\SunCityWall\\SunCityWall.fbx").value();
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
    render::OpaqueMaterial cobblestone_material;
    render::OpaqueMaterial crystal_material;
    render::OpaqueMaterial mud_material;
    render::OpaqueMaterial mudroad_material;
    render::OpaqueMaterial stone_material;
    render::OpaqueMaterial stone_material2;
    cobblestone_material.reset();
    crystal_material.reset();
    mud_material.reset();
    mudroad_material.reset();
    stone_material.reset();
    stone_material2.reset();
    {
        {
            cobblestone_material.albedo_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets\\textures\\Cobblestone\\Cobblestone_albedo.dds");
            cobblestone_material.normal_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets\\textures\\Cobblestone\\Cobblestone_normal.dds");
            cobblestone_material.roughness_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets\\textures\\Cobblestone\\Cobblestone_roughness.dds");
            cobblestone_material.reverse_normal_y = true;
            cobblestone_material.UpdateTextureFlags();
        }
        {
            crystal_material.albedo_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets\\textures\\Crystal\\Crystal_albedo.dds");
            crystal_material.normal_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets\\textures\\Crystal\\Crystal_normal.dds");
            crystal_material.UpdateTextureFlags();
        }
        {
            mud_material.albedo_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets\\textures\\Mud\\Mud_albedo.dds");
            mud_material.normal_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets\\textures\\Mud\\Mud_normal.dds");
            mud_material.roughness_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets\\textures\\Mud\\Mud_roughness.dds");
            mud_material.UpdateTextureFlags();
        }
        {
            mudroad_material.albedo_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets\\textures\\MudRoad\\MudRoad_albedo.dds");
            mudroad_material.normal_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets\\textures\\MudRoad\\MudRoad_normal.dds");
            mudroad_material.roughness_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets\\textures\\MudRoad\\MudRoad_roughness.dds");
            mudroad_material.reverse_normal_y = true;
            mudroad_material.UpdateTextureFlags();
        }
        {
            stone_material.albedo_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets\\textures\\Stone\\Stone_albedo.dds");
            stone_material.normal_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets\\textures\\Stone\\Stone_normal.dds");
            stone_material.roughness_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets\\textures\\Stone\\Stone_roughness.dds");
            stone_material.UpdateTextureFlags();
        }
        {
            stone_material2.albedo_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets\\textures\\Stone\\Stone_COLOR.png");
            stone_material2.normal_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets\\textures\\Stone\\Stone_NORM.png");
            stone_material2.roughness_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets\\textures\\Stone\\Stone_ROUGH.png");
            stone_material2.UpdateTextureFlags();
        }
    }
    {
        std::vector<render::OpaqueMaterial> materials = { cobblestone_material, crystal_material, mud_material, mudroad_material, stone_material, stone_material2 };
        for (size_t i = 0; i < materials.size(); i++)
        {
            auto model_id = render::ModelSystem::GetUnitCube();
            auto cube = registry.create();
            auto &transform = registry.emplace<TransformComponent>(cube);
            transform.position = vec3{ (int32_t)i - (int32_t)materials.size() / 2, 0, -8 };
            transform.scale = vec3{ 1 };
            transform.UpdateMatrices();
            render::ModelSystem::instance().AddOpaqueInstance(model_id, registry, cube, { materials[i] });
        }
    }
    {
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                auto model_id = render::ModelSystem::GetUnitSphereFlat();
                auto sphere = registry.create();
                auto &transform = registry.emplace<TransformComponent>(sphere);
                transform.position = vec3{ i - 5, j, 5 };
                transform.scale = vec3{ 0.375f };
                transform.UpdateMatrices();
                render::OpaqueMaterial material;
                material.reset();
                material.albedo_color = vec3{ 1 };
                material.reflective_color = vec3{ 0 };
                material.metalness_value = mix(0.001f, 1.0f, float(i) / 9.0f);
                material.roughness_value = mix(0.001f, 1.0f, float(j) / 9.0f);
                material.UpdateTextureFlags();
                material.uv_multiplier = vec2{ 1 };
                render::ModelSystem::instance().AddOpaqueInstance(model_id, registry, sphere, { material });
            }
        }
    }
    // ------------------------- CUBES -------------------------
    {
        auto model_id = render::ModelSystem::GetUnitCube();
        auto cube = registry.create();
        auto &transform = registry.emplace<TransformComponent>(cube);
        transform.position = vec3{ 0, -0.5f, 0 };
        transform.scale = vec3{ 10,0.1,10 };
        transform.UpdateMatrices();
        render::ModelSystem::instance().AddOpaqueInstance(model_id, registry, cube, { stone_material });
    }
    {
        auto model_id = render::ModelSystem::GetUnitCube();
        auto cube = registry.create();
        auto &transform = registry.emplace<TransformComponent>(cube);
        transform.position = vec3{ -5, 4.5f, 0 };
        transform.scale = vec3{ 10,0.1,10 };
        transform.rotation = QuaternionFromEuler(0.0f, 0.0f, radians(90.0f));
        transform.UpdateMatrices();
        render::ModelSystem::instance().AddOpaqueInstance(model_id, registry, cube, { stone_material });
    }
    // ------------------------- LIGHTS -------------------------
    {
        auto model_id = render::ModelSystem::GetUnitSphereFlat();
        auto entity = registry.create();
        auto &transform = registry.emplace<TransformComponent>(entity);
        transform.scale = vec3{ 0.15f };
        transform.position = vec3{ 0, 3, -2 };
        transform.UpdateMatrices();
        auto &point_light = registry.emplace<PointLight>(entity);
        point_light.color = vec3{ 0.988, 0.933, 0.655 };
        point_light.power = 4e1f;
        render::ModelSystem::instance().AddEmissiveInstance(model_id, registry, entity, { render::EmissiveMaterial(point_light.color, point_light.power) });
    }
    if (true) {
        auto model_id = render::ModelSystem::GetUnitSphereFlat();
        auto entity = registry.create();
        auto &transform = registry.emplace<TransformComponent>(entity);
        transform.scale = vec3{ 0.15f };
        transform.position = vec3{ 0, 3, 2 };
        transform.UpdateMatrices();
        auto &point_light = registry.emplace<PointLight>(entity);
        point_light.color = vec3{ 0.988, 0.733, 0.555 };
        point_light.power = 1.5e2f;
        render::ModelSystem::instance().AddEmissiveInstance(model_id, registry, entity, { render::EmissiveMaterial(point_light.color, point_light.power) });
    }
    if (true) {
        auto model_id = render::ModelSystem::GetUnitSphereFlat();
        auto entity = registry.create();
        auto &transform = registry.emplace<TransformComponent>(entity);
        transform.scale = vec3{ 0.15f };
        transform.position = vec3{ 0, 2, -8 };
        transform.UpdateMatrices();
        auto &point_light = registry.emplace<PointLight>(entity);
        point_light.color = vec3{ 0.988, 0.933, 0.455 };
        point_light.power = 2e2f;
        render::ModelSystem::instance().AddEmissiveInstance(model_id, registry, entity, { render::EmissiveMaterial(point_light.color, point_light.power) });
    }
    SkyboxManager::LoadSkybox(registry, std::filesystem::current_path() / "assets/textures/skyboxes/night_street/night_street.dds");
    render::ModelSystem::instance().OnInstancesUpdated(registry);
    camera_movement::RegisterKeyCallbacks();
    object_editor::RegisterKeyCallbacks();
    auto &input = *InputLayer::instance();
    input.AddTickKeyCallback({ Key::KEY_PLUS }, [this] (float dt, InputLayer::KeySeq const &, uint32_t) { exposure_ += dt; });
    input.AddTickKeyCallback({ Key::KEY_MINUS }, [this] (float dt, InputLayer::KeySeq const &, uint32_t) { exposure_ -= dt; });
    input.AddTickKeyCallback({ Key::KEY_NUMPAD_MINUS }, [this] (float dt, InputLayer::KeySeq const &, uint32_t) { exposure_ -= dt; });
    input.AddTickKeyCallback({ Key::KEY_NUMPAD_PLUS }, [this] (float dt, InputLayer::KeySeq const &, uint32_t) { exposure_ += dt; });
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

    auto const &camera = Engine::scene()->main_camera->camera();
    renderer_->per_frame.view = camera.view;
    renderer_->per_frame.projection = camera.projection;
    renderer_->per_frame.view_projection = camera.view_projection;
    renderer_->per_frame.inv_view = camera.inv_view;
    renderer_->per_frame.inv_projection = camera.inv_projection;
    renderer_->per_frame.inv_view_projection = camera.inv_view_projection;
    renderer_->per_frame.mouse_position = vec2{ InputLayer::instance()->mouse_position() };
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