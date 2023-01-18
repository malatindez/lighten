#include "controller.hpp"
#include "camera-movement.hpp"
#include "object-editor.hpp"
#include "render/renderer.hpp"
#include "scene-viewer.hpp"
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
#ifdef _DEBUG
    ImGui::Begin("Debug info");
    static int calls_amount = 100;
    ImGui::InputInt("Calls amount", &calls_amount);
    static float seconds_amount = 1;
    ImGui::InputFloat("Seconds amount", &seconds_amount);
    auto &update_measurer = Engine::Get().update_measurer;
    auto &render_measurer = Engine::Get().render_measurer;
    auto &tick_measurer = Engine::Get().tick_measurer;

    ImGui::Text("Average Engine::Update() time: %.3f ms", 1000 * update_measurer.avg());
    ImGui::Text("Average Engine::Update() time over the last %d calls: %.3f ms", calls_amount, 1000 * update_measurer.avg(calls_amount));
    ImGui::Text("Average Engine::Update() time over the last %.2f seconds: %.3f ms", seconds_amount, 1000 * update_measurer.avg_over_the_last(seconds_amount));
    ImGui::Text("Average Engine::Update() time percentage over the last %.2f seconds: %.3f%%", seconds_amount, update_measurer.avg_over_the_last_limited(seconds_amount) * 100);
    ImGui::Text("Average Engine::Update() amount of calls over the last %.2f seconds: %llu", std::min(seconds_amount, update_measurer.elapsed()), update_measurer.amount_of_calls(seconds_amount));

    ImGui::Text("Average Engine::Tick() time: %.3f ms", 1000 * tick_measurer.avg());
    ImGui::Text("Average Engine::Tick() time over the last %d calls: %.3f ms", calls_amount, 1000 * tick_measurer.avg(calls_amount));
    ImGui::Text("Average Engine::Tick() time over the last %.2f seconds: %.3f ms", seconds_amount, 1000 * tick_measurer.avg_over_the_last(seconds_amount));
    ImGui::Text("Average Engine::Tick() time percentage over the last %.2f seconds: %.3f%%", seconds_amount, tick_measurer.avg_over_the_last_limited(seconds_amount) * 100);
    ImGui::Text("Average Engine::Tick() amount of calls over the last %.2f seconds: %llu", std::min(seconds_amount, tick_measurer.elapsed()), tick_measurer.amount_of_calls(seconds_amount));

    ImGui::Text("Average Engine::Render() time: %.3f ms", 1000 * render_measurer.avg());
    ImGui::Text("Average Engine::Render() time over the last %d calls: %.3f ms", calls_amount, 1000 * render_measurer.avg(calls_amount));
    ImGui::Text("Average Engine::Render() time over the last %.2f seconds: %.3f ms", seconds_amount, 1000 * render_measurer.avg_over_the_last(seconds_amount));
    ImGui::Text("Average Engine::Render() time percentage over the last %.2f seconds: %.3f%%", seconds_amount, render_measurer.avg_over_the_last_limited(seconds_amount) * 100);
    ImGui::Text("Average Engine::Render() amount of calls over the last %.2f seconds: %llu", std::min(seconds_amount, render_measurer.elapsed()), render_measurer.amount_of_calls(seconds_amount));
    ImGui::End();
#endif
    ImGui::Begin("Main settings");
    ImGui::Checkbox("Dynamic shadows: ", &camera_movement::dynamic_shadows);
    ImGui::SliderFloat("Shadows update interval##shadows-update-interval",
                       &Engine::scene()->renderer->light_render_system().shadow_map_update_interval(), 0, 1, "%.3f");

    // make button and text field to set the camera fov
    static float fov = Engine::scene()->main_camera->fovy();
    ImGui::SliderFloat("FOV: ", &fov, 0.0f, radians(180.0f));
    if (Engine::scene()->main_camera->fovy() != fov)
    {
        Engine::scene()->main_camera->camera().fovy_ = fov;
        Engine::scene()->main_camera->UpdateProjectionMatrix();
    }
    auto &camera_controller = *Engine::scene()->main_camera;
    ImGui::Checkbox("Roll enabled##roll-enabled", &camera_controller.roll_enabled());
    ImGui::SliderFloat("Sensivity##sensivity", &camera_controller.sensivity(), 0, 100, "%.3f");
    ImGui::SliderFloat("Move speed##move-speed", &camera_controller.move_speed(), 0, 100, "%.3f");
    ImGui::SliderFloat("Accelerated movement speed##accelerated-speed", &camera_controller.accelerated_speed(), 0, 100, "%.3f");
    ImGui::SliderFloat("Roll speed##roll-speed", &camera_controller.roll_speed(), 0, 100, "%.3f");

    auto hdr_to_ldr = hdr_render_pipeline_->hdr_to_ldr_layer();
    ImGui::SliderFloat("Exposure", &hdr_to_ldr->exposure(), -20.0f, 20.0f);
    ImGui::SliderFloat("Gamma", &hdr_to_ldr->gamma(), 0.0f, 20.0f);
    static float update_limit = Engine::maximum_update_rate();
    ImGui::SliderFloat("Update limit", &update_limit, 1.0f, 1000.0f);
    if (update_limit != Engine::maximum_update_rate())
    {
        Engine::SetMaximumUpdateRate(update_limit);
    }
    static float render_limit = Engine::maximum_framerate();
    ImGui::SliderFloat("Render limit", &render_limit, 5.0f, 1000.0f);
    if (render_limit != Engine::maximum_framerate())
    {
        Engine::SetMaximumFramerate(render_limit);
    }
    static float tick_limit = Engine::maximum_tickrate();
    ImGui::SliderFloat("Tick limit", &tick_limit, 1.0f, 1000.0f);
    if (tick_limit != Engine::maximum_tickrate())
    {
        Engine::SetMaximumTickrate(tick_limit);
    }
#if 0 // msaa disabled with deferred rendering
    static int sample_count = 4;
    if (ImGui::BeginCombo("MSAA sample count", std::to_string(sample_count).c_str()))
    {
        if (ImGui::Selectable("1x")) sample_count = 1;
        if (ImGui::Selectable("2x")) sample_count = 2;
        if (ImGui::Selectable("4x")) sample_count = 4;
        if (ImGui::Selectable("8x")) sample_count = 8;
        if (ImGui::Selectable("16x")) sample_count = 16;
        if (ImGui::Selectable("32x")) sample_count = 32;
        ImGui::EndCombo();
    }
    UINT quality_levels;
    direct3d::api().device->CheckMultisampleQualityLevels(DXGI_FORMAT_R16G16B16A16_FLOAT, sample_count, &quality_levels);
    uint32_t current_sample_count = hdr_render_pipeline_->GetSampleCount();
    if (quality_levels == 0)
    {
        spdlog::warn("MSAA sample count {} is not supported", sample_count);
        sample_count = current_sample_count;
    }
    if ((uint32_t)(sample_count) != current_sample_count) { hdr_render_pipeline_->SetSampleCount(sample_count); }
#endif
#if 0
    static core::math::uivec2 poisson_disk_size = { 0.5f, 0.5f };
    ImGui::InputScalar("Poisson disk size x", ImGuiDataType_U32, &poisson_disk_size.x);
    ImGui::InputScalar("Poisson disk size y", ImGuiDataType_U32, &poisson_disk_size.y);
    static uint32_t k, r;
    ImGui::InputScalar("Poisson disk k", ImGuiDataType_U32, &k);
    ImGui::InputScalar("Poisson disk r", ImGuiDataType_U32, &r);

    if (ImGui::Button("generate poisson texture"))
    {
        std::vector<uint8_t> data;
        data.resize(poisson_disk_size.x * poisson_disk_size.y * 4);
        for (uint32_t i = 0; i < poisson_disk_size.x * poisson_disk_size.y; i++)
        {
            data[i * 4 + 0] = 0;
            data[i * 4 + 1] = 0;
            data[i * 4 + 2] = 0;
            data[i * 4 + 3] = 0;
        }
        auto dots = core::math::random::poisson_disc::Generate(poisson_disk_size, k, r);
        for (auto &dot : dots)
        {
            if (dot.y > poisson_disk_size.y - 1 || dot.x > poisson_disk_size.x - 1) continue;
            data[(uint32_t)(dot.y * poisson_disk_size.x + dot.x) * 4 + 0] = 255;
            data[(uint32_t)(dot.y * poisson_disk_size.x + dot.x) * 4 + 1] = 255;
            data[(uint32_t)(dot.y * poisson_disk_size.x + dot.x) * 4 + 2] = 255;
            data[(uint32_t)(dot.y * poisson_disk_size.x + dot.x) * 4 + 3] = 255;
        }
        TextureId id = TextureManager::LoadTexture(data.data(), poisson_disk_size.x, poisson_disk_size.y, 4);
        spdlog::info("poisson disk texture id: {}", id);
        if (object_editor::selected_scene)
        {
            auto *opaque_component = object_editor::selected_scene->registry.try_get<OpaqueComponent>(object_editor::selected_entity);
            if (opaque_component)
            {
                auto *model_instance = Engine::scene()->renderer->opaque_render_system().GetInstancePtr(opaque_component->model_id);
                if (model_instance)
                {
                    for (size_t i = 0; i < model_instance->model.meshes.size(); i++)
                    {
                        for (auto &material_instance : model_instance->mesh_instances[i].material_instances)
                        {
                            if (auto it = std::ranges::find(material_instance.instances, object_editor::selected_entity); it != material_instance.instances.end())
                            {
                                material_instance.material.albedo_map = TextureManager::GetTextureView(id);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
#endif
    ImGui::End();

    object_editor::OnGuiRender();
    object_editor::OnRender(hdr_render_pipeline_->window()->position(), hdr_render_pipeline_->window()->size());

    scene_viewer::OnGuiRender();
}

Controller::Controller(std::shared_ptr<direct3d::DeferredHDRRenderPipeline> hdr_render_pipeline)
    : hdr_render_pipeline_{ hdr_render_pipeline }
{
    first_scene = std::make_shared<Scene>();
    first_scene->renderer = std::make_unique<render::Renderer>();

    auto &ors = first_scene->renderer->opaque_render_system();
    auto &drs = first_scene->renderer->dissolution_render_system();
    auto &grs = first_scene->renderer->grass_render_system();
    auto &ers = first_scene->renderer->emissive_render_system();
    auto &lrs = first_scene->renderer->light_render_system();
    hdr_render_pipeline->deferred_resolve()->SetBrdfTexture(TextureManager::GetTextureView(std::filesystem::current_path() / "assets/textures/IBL/ibl_brdf_lut.dds"));
    hdr_render_pipeline->deferred_resolve()->SetIrradianceTexture(TextureManager::GetTextureView(std::filesystem::current_path() / "assets/textures/skyboxes/night_street/night_street_irradiance.dds"));
    hdr_render_pipeline->deferred_resolve()->SetPrefilteredTexture(TextureManager::GetTextureView(std::filesystem::current_path() / "assets/textures/skyboxes/night_street/night_street_reflection.dds"));
    auto &registry = first_scene->registry;
    main_camera_entity = registry.create();
    registry.emplace<CameraComponent>(main_camera_entity, CameraComponent());
    registry.emplace<TransformComponent>(main_camera_entity, TransformComponent());
    auto &window_size = hdr_render_pipeline->window()->size();
    first_scene->main_camera = std::make_unique<CameraController>(&registry, main_camera_entity, window_size);
    Engine::SetScene(first_scene);
    Engine::scene()->main_camera->SetWorldOffset(core::math::vec3{ 0.0f, 0.0f, 0.0f });
    Engine::scene()->main_camera->SetWorldAngles(0.0f, 0.0f, 0.0f);
    Engine::scene()->main_camera->SetProjectionMatrix(perspective(45.0f, static_cast<float>(window_size.x) / static_cast<float>(window_size.y), 0.001f, 100.0f));

    int amount = 10;
    for (int i = 0; i < amount; i++)
    {
        auto knight = registry.create();
        auto &game_object = registry.emplace<GameObject>(knight);

        last_created_knight = knight;
        auto &transform = registry.emplace<TransformComponent>(knight);
        uint64_t model_id = ModelLoader::Load("assets\\models\\Knight\\Knight.fbx").value();
        game_object.name = "Knight";
#if 0
        if (i % 4 == 1)
        {
            model_id = ModelLoader::Load("assets\\models\\Samurai\\Samurai.fbx").value();
            game_object.name = "Samurai";
        }
        else if (i % 4 == 2)
        {
            model_id = ModelLoader::Load("assets\\models\\KnightHorse\\KnightHorse.fbx").value();
            game_object.name = "KnightHorse";
        }
        else if (i % 4 == 3)
        {
            model_id = ModelLoader::Load("assets\\models\\SunCityWall\\SunCityWall.fbx").value();
            game_object.name = "SunCityWall";
        }
#endif
        ors.AddInstance(model_id, registry, knight);
        transform.position = vec3{
                                 std::sin(float(i) / amount * 2 * (float)std::numbers::pi),
                                 0,
                                 std::cos(float(i) / amount * 2 * (float)std::numbers::pi) } *
                                 (float(amount) / std::sqrtf((float)amount));
        transform.rotation = QuaternionFromEuler(0.0f, 0.0f, radians(180.0f));
        transform.rotation *= QuaternionFromRotationMatrix(look_at(transform.position, vec3{ 0, 0, 0 }, vec3{ 0, 1, 0 }).as_rmat<3, 3>());
        transform.UpdateMatrices();
    }
    render::OpaqueMaterial cobblestone_material;
    render::OpaqueMaterial crystal_material;
    render::OpaqueMaterial mud_material;
    render::OpaqueMaterial mudroad_material;
    render::OpaqueMaterial stone_material;
    render::OpaqueMaterial blue_metal;
    render::OpaqueMaterial white_half_metal;
    render::OpaqueMaterial white_porcelain;
    render::OpaqueMaterial blue_rubber;

    cobblestone_material.reset();
    crystal_material.reset();
    mud_material.reset();
    mudroad_material.reset();
    stone_material.reset();
    blue_metal.reset();
    white_half_metal.reset();
    white_porcelain.reset();
    blue_rubber.reset();
    {
        {cobblestone_material.albedo_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets\\textures\\Cobblestone\\Cobblestone_albedo.dds");
        cobblestone_material.normal_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets\\textures\\Cobblestone\\Cobblestone_normal.dds");
        cobblestone_material.roughness_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets\\textures\\Cobblestone\\Cobblestone_roughness.dds");
        cobblestone_material.UpdateTextureFlags();
        }
        {
            crystal_material.albedo_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets\\textures\\Crystal\\Crystal_albedo.dds");
            crystal_material.normal_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets\\textures\\Crystal\\Crystal_normal.dds");
            crystal_material.reverse_normal_y = true;
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
            blue_metal.albedo_color = vec3{ 0.1f, 0.1f, 1.0f };
            blue_metal.metalness_value = 1.0f;
            blue_metal.roughness_value = 0.1f;
        }
        {
            white_half_metal.albedo_color = vec3{ 0.9f, 0.9f, 0.9f };
            white_half_metal.metalness_value = 0.5f;
            white_half_metal.roughness_value = 0.25f;
        }
        {
            white_porcelain.albedo_color = vec3{ 0.9f, 0.9f, 0.9f };
            white_porcelain.metalness_value = 0.0f;
            white_porcelain.roughness_value = 0.15f;
        }
        {
            blue_rubber.albedo_color = vec3{ 0.1f, 0.1f, 1.0f };
            blue_rubber.metalness_value = 0.0f;
            blue_rubber.roughness_value = 0.7f;
        }
    }
    {
        entt::entity cubes = registry.create();
        auto &cubes_game_object = registry.emplace<GameObject>(cubes);
        cubes_game_object.name = "Cubes";
        std::vector<render::OpaqueMaterial> materials = { cobblestone_material, crystal_material, mud_material, mudroad_material, stone_material, blue_metal, white_half_metal, white_porcelain, blue_rubber };
        std::vector<std::string> material_names = { "Cobblestone cube", "Crystal cube", "Mud cube", "MudRoad cube", "Stone cube", "BlueMetal cube", "WhiteHalfMetal cube", "WhitePorcelain cube", "BlueRubber cube" };
        for (size_t i = 0; i < materials.size(); i++)
        {
            auto model_id = render::ModelSystem::GetUnitCube();
            auto cube = registry.create();
            auto &game_object = registry.emplace<GameObject>(cube);
            game_object.name = material_names[i];
            game_object.parent = cubes;
            cubes_game_object.children.push_back(cube);
            auto &transform = registry.emplace<TransformComponent>(cube);
            transform.position = vec3{ (int32_t)i - (int32_t)materials.size() / 2, 0, -8 };
            transform.scale = vec3{ 1 };
            transform.UpdateMatrices();
            ors.AddInstance(model_id, registry, cube, { materials[i] });
        }
    }
    if (false)
    {
        entt::entity spheres = registry.create();
        auto &spheres_game_object = registry.emplace<GameObject>(spheres);
        spheres_game_object.name = "Spheres";
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                auto model_id = render::ModelSystem::GetUnitSphereFlat();
                auto sphere = registry.create();
                auto &game_object = registry.emplace<GameObject>(sphere);
                game_object.name = "Sphere";
                game_object.parent = spheres;
                spheres_game_object.children.push_back(sphere);
                auto &transform = registry.emplace<TransformComponent>(sphere);
                transform.position = vec3{ i - 5, j, 5 };
                transform.scale = vec3{ 0.375f };
                transform.UpdateMatrices();
                render::OpaqueMaterial material;
                material.reset();
                material.albedo_color = vec3{ 1 };
                material.metalness_value = mix(0.001f, 1.0f, float(i) / 9.0f);
                material.roughness_value = mix(0.001f, 1.0f, float(j) / 9.0f);
                material.UpdateTextureFlags();
                material.uv_multiplier = vec2{ 1 };
                ors.AddInstance(model_id, registry, sphere, { material });
            }
        }
    }
    // ------------------------- CUBES -------------------------
    {
        entt::entity floor = registry.create();
        auto &floor_game_object = registry.emplace<GameObject>(floor);
        floor_game_object.name = "Floor";
        for (int i = 0; i < 4; i++)
        {
            auto model_id = render::ModelSystem::GetUnitCube();
            auto cube = registry.create();
            auto &game_object = registry.emplace<GameObject>(cube);
            game_object.name = "Floor part #" + std::to_string(i);
            game_object.parent = floor;
            floor_game_object.children.push_back(cube);
            auto &transform = registry.emplace<TransformComponent>(cube);
            transform.position = vec3{ 0, -0.5f, 0 } + vec3{ i < 2 ? -1 : 1, 0, i % 2 == 0 ? -1 : 1 } *2.5f;
            transform.scale = vec3{ 5, 0.1, 5 };
            transform.UpdateMatrices();
            ors.AddInstance(model_id, registry, cube, { stone_material });
        }
    }
    {
        auto model_id = render::ModelSystem::GetUnitCube();
        auto wall = registry.create();
        auto &wall_game_object = registry.emplace<GameObject>(wall);
        wall_game_object.name = "Wall";
        auto &transform = registry.emplace<TransformComponent>(wall);
        transform.position = vec3{ -5, 4.5f, 0 };
        transform.scale = vec3{ 10, 0.1, 10 };
        transform.rotation = QuaternionFromEuler(0.0f, 0.0f, radians(90.0f));
        transform.UpdateMatrices();
        ors.AddInstance(model_id, registry, wall, { stone_material });
    }
    // ------------------------- LIGHTS -------------------------
    entt::entity lights = registry.create();
    auto &lights_game_object = registry.emplace<GameObject>(lights);
    if (false)
    {
        auto model_id = render::ModelSystem::GetUnitSphereFlat();
        auto entity = registry.create();
        auto &game_object = registry.emplace<GameObject>(entity);
        game_object.name = "White point light";
        game_object.parent = lights;
        lights_game_object.children.push_back(entity);
        auto &transform = registry.emplace<TransformComponent>(entity);
        transform.scale = vec3{ 0.15f };
        transform.position = vec3{ 0, 3, -2 };
        transform.UpdateMatrices();
        auto &point_light = registry.emplace<PointLight>(entity);
        point_light.color = vec3{ 0.988, 0.933, 0.655 };
        point_light.power = 1e3f;
        ers.AddInstance(model_id, registry, entity, { render::EmissiveMaterial(point_light.color, point_light.power) });
    }
    if (true)
    {
        auto model_id = render::ModelSystem::GetUnitSphereFlat();
        auto entity = registry.create();
        auto &game_object = registry.emplace<GameObject>(entity);
        game_object.name = "Red point light";
        game_object.parent = lights;
        lights_game_object.children.push_back(entity);
        auto &transform = registry.emplace<TransformComponent>(entity);
        transform.scale = vec3{ 0.15f };
        transform.position = vec3{ 0, 3, 2 };
        transform.UpdateMatrices();
        auto &point_light = registry.emplace<PointLight>(entity);
        point_light.color = vec3{ 0.988, 0.233, 0.255 };
        point_light.power = 2.5e3f;
        point_light.casts_shadows = true;
        ers.AddInstance(model_id, registry, entity, { render::EmissiveMaterial(point_light.color, point_light.power) });
    }
    if (false)
    {
        auto model_id = render::ModelSystem::GetUnitSphereFlat();
        auto entity = registry.create();
        auto &game_object = registry.emplace<GameObject>(entity);
        game_object.name = "Green point light";
        game_object.parent = lights;
        lights_game_object.children.push_back(entity);
        auto &transform = registry.emplace<TransformComponent>(entity);
        transform.scale = vec3{ 0.15f };
        transform.position = vec3{ 0, 2, -8 };
        transform.UpdateMatrices();
        auto &point_light = registry.emplace<PointLight>(entity);
        point_light.color = vec3{ 0.01, 0.933, 0.255 };
        point_light.power = 3e3f;
        point_light.casts_shadows = true;
        ers.AddInstance(model_id, registry, entity, { render::EmissiveMaterial(point_light.color, point_light.power) });
    }
    if (true)
    {
        for (int i = 0; i < 24; i++)
        {
            auto model_id = render::ModelSystem::GetUnitSphereFlat();
            auto entity = registry.create();
            auto &game_object = registry.emplace<GameObject>(entity);
            game_object.name = "Point light";
            game_object.parent = lights;
            lights_game_object.children.push_back(entity);
            auto &transform = registry.emplace<TransformComponent>(entity);
            transform.scale = vec3{ 0.1f };
            transform.position = vec3{ 0, 0.025f, 0 };
            transform.position += vec3{ std::cosf(float(i) / 24 * 2 * std::numbers::pi_v<float>), 0, std::sinf(float(i) / 24 * 2 * std::numbers::pi_v<float>) } *1.0f;
            transform.UpdateMatrices();
            auto &point_light = registry.emplace<PointLight>(entity);
            point_light.color = vec3{ (i + 1) % 2, (i + 1) % 3, (i + 1) % 5 };
            point_light.color = normalize(point_light.color);
            point_light.power = 25.0f;
            point_light.casts_shadows = false;
            ers.AddInstance(model_id, registry, entity, { render::EmissiveMaterial(point_light.color, point_light.power) });
        }
    }

    // add directional light
    if (true)
    {
        auto model_id = render::ModelSystem::GetUnitSphereFlat();
        auto entity = registry.create();
        auto &transform = registry.emplace<TransformComponent>(entity);
        auto &game_object = registry.emplace<GameObject>(entity);
        game_object.name = "Directional light";
        game_object.parent = lights;
        lights_game_object.children.push_back(entity);
        transform.scale = vec3{ 0.15f };
        transform.position = vec3{ 0, 10, 0 };
        transform.rotation = QuaternionFromEuler(vec3{ radians(-120.0f), radians(-30.0f), radians(-60.0f) });
        transform.UpdateMatrices();
        auto &directional_light = registry.emplace<DirectionalLight>(entity);
        directional_light.color = vec3{ 0.988, 0.933, 0.455 };
        directional_light.power = 50;
        directional_light.solid_angle = 0.25f;
        ers.AddInstance(model_id, registry, entity, { render::EmissiveMaterial(directional_light.color, directional_light.power) });
    }
    // add spot light
    if (false)
    {
        auto model_id = render::ModelSystem::GetUnitSphereFlat();
        auto entity = registry.create();
        auto &transform = registry.emplace<TransformComponent>(entity);

        auto &game_object = registry.emplace<GameObject>(entity);
        game_object.name = "Spot light";
        game_object.parent = lights;
        lights_game_object.children.push_back(entity);

        transform.scale = vec3{ 0.15f };
        transform.position = vec3{ 0, 3, 0 };
        transform.rotation = QuaternionFromEuler(vec3{ radians(-120.0f), radians(-30.0f), radians(-60.0f) });
        transform.UpdateMatrices();
        auto &spot_light = registry.emplace<SpotLight>(entity);
        spot_light.color = vec3{ 0.988, 0.933, 0.455 };
        spot_light.power = 10;
        spot_light.inner_cutoff = radians(10.0f);
        spot_light.outer_cutoff = radians(20.0f);
        ers.AddInstance(model_id, registry, entity, { render::EmissiveMaterial(spot_light.color, spot_light.power) });
    }
    auto prs_texture_path = std::filesystem::current_path() / "assets/textures/smoke";
    first_scene->renderer->particle_render_system().botbf = TextureManager::GetTextureView(prs_texture_path / "BotBF.tga");
    first_scene->renderer->particle_render_system().scatter = TextureManager::GetTextureView(prs_texture_path / "emission+scatter.tga");
    first_scene->renderer->particle_render_system().emva1 = TextureManager::GetTextureView(prs_texture_path / "EMVA1.tga");
    first_scene->renderer->particle_render_system().emva2 = TextureManager::GetTextureView(prs_texture_path / "EMVA2.tga");
    first_scene->renderer->particle_render_system().rlt = TextureManager::GetTextureView(prs_texture_path / "RLT.tga");
    first_scene->renderer->particle_render_system().atlas_size = { 8, 8 };

    // add particle emitters
    if (true)
    {
        auto model_id = render::ModelSystem::GetUnitSphereFlat();
        auto entity = registry.create();
        auto &game_object = registry.emplace<GameObject>(entity);
        game_object.name = "Particle emitter";
        auto &transform = registry.emplace<TransformComponent>(entity);
        transform.scale = vec3{ 0.15f };
        transform.position = vec3{ -3.5, 2, -8 };
        transform.rotation = QuaternionFromEuler(vec3{ 0, 0, radians(-90.0f) });
        transform.UpdateMatrices();

        auto &particle_emitter = registry.emplace<ParticleEmitter>(entity);
        particle_emitter.position_yaw_pitch_range = vec4{ -numbers::pi / 4, -numbers::pi / 4, numbers::pi / 4, numbers::pi / 4 };
        particle_emitter.position_radius = vec2{ 0.25, 0.5f };
        particle_emitter.velocity_yaw_pitch_range = vec4{ -numbers::pi / 16, -numbers::pi / 16, numbers::pi / 16, numbers::pi / 16 };
        particle_emitter.velocity_radius = vec2{ 0.15f, 0.4f };
        particle_emitter.base_diffuse_color = vec4{ 1.288, 1.133, 1.055, 1.0f };
        particle_emitter.diffuse_variation = vec4{ 0.2f, 0.2f, 0.2f, 0.0f };
        particle_emitter.particle_lifespan_range = vec2{ 0.5f, 15.0f };
        particle_emitter.thickness_range = vec2{ 0.0f, 0.05f };
        particle_emitter.begin_size_range = vec2{ 0.01f, 0.15f };
        particle_emitter.end_size_range = vec2{ 0.0f, 0.3f };
        particle_emitter.mass_range = vec2{ 0.1f, 0.5f };
        particle_emitter.emit_rate = 10;
        particle_emitter.rotation_range = vec2{ 0.0f, 2 * numbers::pi };
        particle_emitter.rotation_speed_range = vec2{ 0.0f, 0.1 * numbers::pi };
        particle_emitter.particle_acceleration = vec3{ 0.0f };
        particle_emitter.maximum_amount_of_particles = 200;
        ors.AddInstance(model_id, registry, entity, { white_porcelain });
    }
    if (true)
    {
        auto model_id = render::ModelSystem::GetUnitSphereFlat();
        auto entity = registry.create();
        auto &game_object = registry.emplace<GameObject>(entity);
        game_object.name = "Particle emitter";
        auto &transform = registry.emplace<TransformComponent>(entity);
        transform.scale = vec3{ 0.15f };
        transform.position = vec3{ 4, 2, -8 };
        transform.rotation = QuaternionFromEuler(vec3{ 0, 0, radians(-90.0f) });
        transform.UpdateMatrices();

        auto &particle_emitter = registry.emplace<ParticleEmitter>(entity);
        particle_emitter.position_yaw_pitch_range = vec4{ -numbers::pi / 4, -numbers::pi / 4, numbers::pi / 4, numbers::pi / 4 };
        particle_emitter.position_radius = vec2{ 0.25, 0.5f };
        particle_emitter.velocity_yaw_pitch_range = vec4{ -numbers::pi / 8, -numbers::pi / 8, numbers::pi / 8, numbers::pi / 8 };
        particle_emitter.velocity_radius = vec2{ 5.0f, 10.0f };
        particle_emitter.base_diffuse_color = vec4{ 1.988, 1.933, 1.455, 1.0f };
        particle_emitter.diffuse_variation = vec4{ 0.8f, 0.5f, 0.5f, 0.0f };
        particle_emitter.particle_lifespan_range = vec2{ 25.0f, 100.0f };
        particle_emitter.thickness_range = vec2{ 0.0f, 0.05f };
        particle_emitter.begin_size_range = vec2{ 0.01f, 0.25f };
        particle_emitter.end_size_range = vec2{ 4.0f, 10.0f };
        particle_emitter.mass_range = vec2{ 0.1f, 0.5f };
        particle_emitter.emit_rate = 3;
        particle_emitter.rotation_range = vec2{ 0.0f, 2 * numbers::pi };
        particle_emitter.rotation_speed_range = vec2{ 0.0f, 0.1 * numbers::pi };
        particle_emitter.particle_acceleration = vec3{ 0.0f };
        particle_emitter.maximum_amount_of_particles = 300;
        ors.AddInstance(model_id, registry, entity, { white_porcelain });
    }
    if (true)
    {
        auto grass = registry.create();
        auto &game_object = registry.emplace<GameObject>(grass);
        game_object.name = "Grass";
        auto &transform = registry.emplace<TransformComponent>(grass);
        transform.scale = vec3{ 2.0f, 0.1f, 2.0f };
        transform.position = vec3{ 0, 0, -12 };
        transform.UpdateMatrices();

        render::OpaqueMaterial grass_floor_material;
        grass_floor_material.reset();
        grass_floor_material.albedo_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets/textures/grass_floor/grass_terrain_surface.dds");
        grass_floor_material.metalness_value = 0.0001f;
        grass_floor_material.roughness_value = 1.0f;
        grass_floor_material.UpdateTextureFlags();

        auto model_id = render::ModelSystem::GetUnitCube();
        ors.AddInstance(model_id, registry, grass, { grass_floor_material });

        render::GrassMaterial grass_material;
        grass_material.reset();
        grass_material.albedo_texture = TextureManager::GetTextureView(std::filesystem::current_path() / "assets/textures/grass/ribbon_grass/Billboard_4K_Albedo.dds");
        grass_material.ao_texture = TextureManager::GetTextureView(std::filesystem::current_path() / "assets/textures/grass/ribbon_grass/Billboard_4K_AO.dds");
        grass_material.bump_texture = TextureManager::GetTextureView(std::filesystem::current_path() / "assets/textures/grass/ribbon_grass/Billboard_4K_Bump.dds");
        grass_material.cavity_texture = TextureManager::GetTextureView(std::filesystem::current_path() / "assets/textures/grass/ribbon_grass/Billboard_4K_Cavity.dds");
        grass_material.displacement_texture = TextureManager::GetTextureView(std::filesystem::current_path() / "assets/textures/grass/ribbon_grass/Billboard_4K_Displacement.dds");
        grass_material.gloss_texture = TextureManager::GetTextureView(std::filesystem::current_path() / "assets/textures/grass/ribbon_grass/Billboard_4K_Gloss.dds");
        grass_material.normal_texture = TextureManager::GetTextureView(std::filesystem::current_path() / "assets/textures/grass/ribbon_grass/Billboard_4K_Normal.dds");
        grass_material.opacity_texture = TextureManager::GetTextureView(std::filesystem::current_path() / "assets/textures/grass/ribbon_grass/Billboard_4K_Opacity.dds");
        grass_material.roughness_texture = TextureManager::GetTextureView(std::filesystem::current_path() / "assets/textures/grass/ribbon_grass/Billboard_4K_Roughness.dds");
        grass_material.specular_texture = TextureManager::GetTextureView(std::filesystem::current_path() / "assets/textures/grass/ribbon_grass/Billboard_4K_Specular.dds");
        grass_material.translucency_texture = TextureManager::GetTextureView(std::filesystem::current_path() / "assets/textures/grass/ribbon_grass/Billboard_4K_Translucency.dds");
        grass_material.UpdateTextureFlags();

        grass_material.wind_vector = { 1.0f, 0.0f, 1.0f };
        grass_material.wind_amplitude = math::radians(45.0f);
        grass_material.wind_wavenumber = 2.0f;
        grass_material.wind_frequency = 1.0f;

        core::math::vec4 texture_size = { 4096, 4096, 4096, 4096 };
        grass_material.atlas_data = {
            core::math::vec4{82, 69, 2277, 1736} / texture_size,
            core::math::vec4{2474, 90, 4017, 1361} / texture_size,
            core::math::vec4{377, 1810, 1761, 2697} / texture_size,
            core::math::vec4{2228, 1480, 3894, 2893} / texture_size,
            core::math::vec4{76, 2746, 2217, 4065} / texture_size,
            core::math::vec4{2651, 3005, 3774, 4033} / texture_size,
        };
        grass_material.planes_count = 2;
        grass_material.section_count = 4;
        auto atlas_data_copy = grass_material.atlas_data;
        auto material_id = grs.AddMaterial(std::move(grass_material));

        auto &grass_component = registry.emplace<GrassComponent>(grass);
        grass_component.material_id = material_id;
        grass_component.spawn_range = { 1.8f, 1.8f };
        grass_component.grass_size_range = { 0.075f, 0.1f };
        grass_component.initial_offset = { 0.0f, 0.11f, 0.0f };

        grass_component.min_distance = 0.1f;
        grass_component.max_attempts = 64;
        grass_component.Initialize(atlas_data_copy);
    }
    SkyboxManager::LoadSkybox(registry, std::filesystem::current_path() / "assets/textures/skyboxes/night_street/night_street.dds");
    first_scene->ScheduleInstanceUpdate();
    camera_movement::RegisterKeyCallbacks();
    object_editor::RegisterKeyCallbacks();
    auto &input = *InputLayer::instance();
    auto &exposure = hdr_render_pipeline_->hdr_to_ldr_layer()->exposure();
    input.AddTickKeyCallback({ Key::KEY_PLUS }, [this, &exposure] (float dt, InputLayer::KeySeq const &, uint32_t)
                             { exposure += dt; });
    input.AddTickKeyCallback({ Key::KEY_MINUS }, [this, &exposure] (float dt, InputLayer::KeySeq const &, uint32_t)
                             { exposure -= dt; });
    input.AddTickKeyCallback({ Key::KEY_NUMPAD_MINUS }, [this, &exposure] (float dt, InputLayer::KeySeq const &, uint32_t)
                             { exposure -= dt; });
    input.AddTickKeyCallback({ Key::KEY_NUMPAD_PLUS }, [this, &exposure] (float dt, InputLayer::KeySeq const &, uint32_t)
                             { exposure += dt; });
    input.AddUpdateKeyCallback(
        { Key::KEY_F },
        [&] (InputLayer::KeySeq const &, uint32_t)
        {
            static utils::HighResolutionTimer timer;
            const float kDelay = 1.0f / 15.0f;
            if (ImGui::GetIO().WantCaptureMouse)
            {
                return;
            }
            if (timer.elapsed() < kDelay)
            {
                return;
            }

            auto &input = *InputLayer::instance();
            auto scene = Engine::scene();
            Ray ray = scene->main_camera->PixelRaycast(vec2{ input.mouse_position() });
            MeshIntersection nearest;
            nearest.reset();
            std::optional<entt::entity> entity = render::ModelSystem::FindIntersection(scene->registry, ray, nearest);
            if (!entity.has_value())
            {
                return;
            }
            auto *opaque_component = registry.try_get<components::OpaqueComponent>(*entity);
            if (opaque_component == nullptr)
            {
                return;
            }
            auto &transform = scene->registry.get<TransformComponent>(*entity);
            DecalComponent::Decal decal;
            uint32_t mesh_id = std::numeric_limits<uint32_t>::max();
            auto *model_instance = Engine::scene()->renderer->opaque_render_system().GetInstancePtr(opaque_component->model_id);
            {
                // TODO:
                // make FindIntersection return mesh and model id
                // this is a bit of an overhead
                if (model_instance == nullptr)
                {
                    return;
                }
                for (size_t i = 0; i < model_instance->model.meshes.size(); i++)
                {
                    if (&model_instance->model.meshes[i].mesh == nearest.mesh_ptr)
                    {
                        mesh_id = (uint32_t)i;
                    }
                }
                if (mesh_id == std::numeric_limits<uint32_t>::max())
                {
                    return;
                }
            }
            static ID3D11ShaderResourceView *texture = core::TextureManager::GetTextureView(std::filesystem::current_path() / "assets/textures/decal.dds");
            decal.normal_opacity_map = texture;
            decal.mesh_transform = model_instance->model.meshes[mesh_id].mesh_to_model;
            mat4 inv_mat = model_instance->model.meshes[mesh_id].inv_mesh_to_model * transform.inv_model;
            decal.relative_position = (inv_mat * vec4(nearest.point, 1.0f)).xyz;

            decal.texture_angle = std::uniform_real_distribution(0.0f, 2.0f * core::math::numbers::pi_v<float>)(core::Engine::random_engine());

            decal.relative_scale = vec3{ 0.1f } / transform.scale;
            decal.base_color = random::RandomVector3(core::Engine::random_engine(), 0.25f, 1.0f);
            decal.roughness = 1.0f;
            decal.metalness = 0.01f;
            decal.transmittance = 0.0f;
            decal.ambient_occlusion = 1.0f;
            auto &decal_component = scene->registry.get_or_emplace<DecalComponent>(*entity);
            decal_component.decals.emplace_back(std::move(decal));
            timer.reset();
            scene->renderer->decal_render_system().ScheduleInstanceUpdate();
        },
        true);
    input.AddUpdateKeyCallback(InputLayer::KeySeq{ engine::core::Key::KEY_N },
                               [&] (InputLayer::KeySeq const &, uint32_t count)
                               {
                                   if (count == std::numeric_limits<uint32_t>::max())
                                   {
                                       return;
                                   }
                                   static utils::SteadyTimer timer;
                                   static float kDeleteLimit = 0.5f;
                                   if (timer.elapsed() < kDeleteLimit)
                                   {
                                       return;
                                   }
                                   timer.reset();
                                   auto &registry = Engine::scene()->registry;
                                   auto &drs = Engine::scene()->renderer->dissolution_render_system();
                                   auto knight = registry.create();

                                   auto &transform = registry.emplace<TransformComponent>(knight);
                                   transform.position = Engine::scene()->main_camera->position() + Engine::scene()->main_camera->forward() * 2.0f;
                                   transform.position -= Engine::scene()->main_camera->up();
                                   math::mat3 rotation_matrix{
                                       Engine::scene()->main_camera->right(),
                                       -Engine::scene()->main_camera->up(),
                                       -Engine::scene()->main_camera->forward() };
                                   rotation_matrix = math::inverse(math::rtranspose(rotation_matrix));
                                   transform.rotation = math::QuaternionFromRotationMatrix(rotation_matrix);
                                   transform.UpdateMatrices();
                                   uint64_t model_id = ModelLoader::Load("assets\\models\\Knight\\Knight.fbx").value();
                                   drs.AddInstance(model_id, registry, knight, std::uniform_real_distribution<float>(3.0f, 15.0f)(Engine::random_engine()));
                                   Engine::scene()->renderer->dissolution_render_system().ScheduleInstanceUpdate();
                                   Engine::scene()->renderer->light_render_system().ScheduleShadowMapUpdate();
                               });

    input.AddUpdateKeyCallback(
        { Key::KEY_DELETE },
        [&] (InputLayer::KeySeq const &, uint32_t)
        {
            static utils::HighResolutionTimer timer;
            const float kDelay = 0.5f;
            if (ImGui::GetIO().WantCaptureMouse)
            {
                return;
            }
            if (timer.elapsed() < kDelay)
            {
                return;
            }
            timer.reset();
            auto scene = Engine::scene();
            Ray ray = scene->main_camera->PixelRaycast(vec2{ input.mouse_position() });
            MeshIntersection nearest;
            nearest.reset();
            std::optional<entt::entity> entity = render::ModelSystem::FindIntersection(scene->registry, ray, nearest);
            if (!entity.has_value())
            {
                return;
            }

            auto *opaque = registry.try_get<components::OpaqueComponent>(entity.value());
            // TODO:
            // Make better system to get/fetch models and their materials
            // This is a hack, we should have a more flexible way to do this
            if (opaque != nullptr)
            {
                float lifetime = std::uniform_real_distribution(1.0f, 5.0f)(core::Engine::random_engine());
                auto &model_instance = ors.GetInstance(opaque->model_id);
                std::vector<render::OpaqueMaterial *> materials;
                for (auto &mesh_instance : model_instance.mesh_instances)
                {
                    for (auto &material_instance : mesh_instance.material_instances)
                    {
                        auto it = std::find(material_instance.instances.begin(), material_instance.instances.end(), entity.value());
                        if (it != material_instance.instances.end())
                        {
                            material_instance.instances.erase(it);
                            materials.push_back(&material_instance.material);
                            break;
                        }
                    }
                }
                std::vector<render::DissolutionMaterial> dissolution_materials;

                std::transform(
                    materials.begin(),
                    materials.end(),
                    std::back_inserter(dissolution_materials),
                    [](render::OpaqueMaterial *material) __lambda_force_inline->render::DissolutionMaterial
                    {
                       auto rv = render::DissolutionMaterial::FromOpaqueMaterial(*material);
                       rv.emissive = true;
                       rv.appearing = false;
                       rv.UpdateTextureFlags();
                       return rv;
                    });
                drs.AddInstance(opaque->model_id, registry, entity.value(), lifetime, std::move(dissolution_materials));
                drs.ScheduleInstanceUpdate();

                registry.erase<components::OpaqueComponent>(entity.value());
                ors.ScheduleInstanceUpdate();
                opaque = nullptr;
            }
        },
        false);
    lrs.ScheduleShadowMapUpdate();
}
void Controller::OnTick([[maybe_unused]] float delta_time)
{
    for (auto const &func : update_callbacks_)
    {
        func(delta_time);
    }
    auto &input = *InputLayer::instance();
    auto scene = Engine::scene();
    camera_movement::OnTick(delta_time);

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
void Controller::OnUpdate()
{
    camera_movement::OnUpdate();
}