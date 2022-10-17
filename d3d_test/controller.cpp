#include "controller.hpp"
#include "camera-movement.hpp"
#include "transform-editor.hpp"
#include "render/renderer.hpp"
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
    auto hdr_to_ldr = hdr_render_pipeline_->hdr_to_ldr_layer();
    ImGui::SliderFloat("Exposure", &hdr_to_ldr->exposure(), -20.0f, 20.0f);
    ImGui::SliderFloat("Gamma", &hdr_to_ldr->gamma(), 0.0f, 20.0f);
    ImGui::SliderFloat("Default AO", &first_scene->renderer->opaque_render_system().ambient_occlusion(), 0.0f, 1.0f);
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
    if (sample_count != current_sample_count) { hdr_render_pipeline_->SetSampleCount(sample_count); }
    auto &lrs = Engine::scene()->renderer->light_render_system();
    auto const &point_lights = lrs.point_light_entities();
    auto const &spot_lights = lrs.spot_light_entities();
    auto const &directional_lights = lrs.directional_light_entities();
    auto const &point_light_matrices = lrs.point_light_shadow_matrices();
    auto const &spot_light_matrices = lrs.spot_light_shadow_matrices();
    auto const &directional_light_matrices = lrs.directional_light_shadow_matrices();
    static UINT view_projection_num = std::numeric_limits<UINT>::max();
    ImGui::InputScalar("View projection num", ImGuiDataType_U32, &view_projection_num, nullptr, nullptr, "%u", ImGuiInputTextFlags_CharsDecimal);
    if (view_projection_num < point_lights.size() * 6)
    {
        view_proj = point_light_matrices.at(point_lights[view_projection_num / 6])[view_projection_num % 6];
        Engine::scene()->main_camera->camera().view_projection = view_proj;
    }
    else if (view_projection_num < point_lights.size() * 6 + spot_lights.size())
    {
        view_proj = spot_light_matrices.at(spot_lights[view_projection_num - point_lights.size() * 6]);
        Engine::scene()->main_camera->camera().view_projection = view_proj;
    }
    else if (view_projection_num < point_lights.size() * 6 + spot_lights.size() + directional_lights.size())
    {
        view_proj = directional_light_matrices.at(directional_lights[view_projection_num - point_lights.size() * 6 - spot_lights.size()]);
        Engine::scene()->main_camera->camera().view_projection = view_proj;
    }

    ImGui::End();
    object_editor::OnGuiRender();
}

struct GizmoOverlay : public Layer, public Layer::HandleRender
{
    std::shared_ptr<direct3d::HDRRenderPipeline> hdr_render_pipeline;
    void OnRender() override
    {
        object_editor::OnRender(hdr_render_pipeline->window()->position(), hdr_render_pipeline->window()->size());
    }
};
Controller::Controller(std::shared_ptr<direct3d::HDRRenderPipeline> hdr_render_pipeline)
    : hdr_render_pipeline_{ hdr_render_pipeline }
{
    first_scene = std::make_shared<Scene>();
    first_scene->renderer = std::make_unique<render::Renderer>();

    auto gizmo_overlay = std::make_shared<GizmoOverlay>();
    gizmo_overlay->hdr_render_pipeline = hdr_render_pipeline_;
    hdr_render_pipeline_->PushOverlay(gizmo_overlay);

    auto &ors = first_scene->renderer->opaque_render_system();
    auto &ers = first_scene->renderer->emissive_render_system();
    ors.SetBrdfTexture(TextureManager::GetTextureView(std::filesystem::current_path() / "assets/textures/IBL/ibl_brdf_lut.dds"));
    ors.SetIrradianceTexture(TextureManager::GetTextureView(std::filesystem::current_path() / "assets/textures/skyboxes/night_street/night_street_irradiance.dds"));
    ors.SetPrefilteredTexture(TextureManager::GetTextureView(std::filesystem::current_path() / "assets/textures/skyboxes/night_street/night_street_reflection.dds"));
    auto &registry = first_scene->registry;
    main_camera_entity = registry.create();
    registry.emplace<CameraComponent>(main_camera_entity, CameraComponent());
    registry.emplace<TransformComponent>(main_camera_entity, TransformComponent());
    auto &window_size = hdr_render_pipeline->window()->size();
    first_scene->main_camera = std::make_unique<CameraController>(&registry, main_camera_entity, window_size);
    Engine::SetScene(first_scene);
    Engine::scene()->main_camera->SetWorldOffset(core::math::vec3{ 0.0f, 0.0f, 0.0f });
    Engine::scene()->main_camera->SetWorldAngles(0.0f, 0.0f, 0.0f);
    Engine::scene()->main_camera->SetProjectionMatrix(perspective(45.0f, float(window_size.x) / float(window_size.y), 0.001f, 100.0f));

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
        ors.AddInstance(model_id, registry, knight);
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
        {
            cobblestone_material.albedo_map = TextureManager::GetTextureView(std::filesystem::current_path() / "assets\\textures\\Cobblestone\\Cobblestone_albedo.dds");
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
        std::vector<render::OpaqueMaterial> materials = { cobblestone_material, crystal_material, mud_material, mudroad_material, stone_material, blue_metal, white_half_metal, white_porcelain, blue_rubber };
        for (size_t i = 0; i < materials.size(); i++)
        {
            auto model_id = render::ModelSystem::GetUnitCube();
            auto cube = registry.create();
            auto &transform = registry.emplace<TransformComponent>(cube);
            transform.position = vec3{ (int32_t)i - (int32_t)materials.size() / 2, 0, -8 };
            transform.scale = vec3{ 1 };
            transform.UpdateMatrices();
            ors.AddInstance(model_id, registry, cube, { materials[i] });
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
        for (int i = 0; i < 4; i++)
        {
            auto model_id = render::ModelSystem::GetUnitCube();
            auto cube = registry.create();
            auto &transform = registry.emplace<TransformComponent>(cube);
            transform.position = vec3{ 0, -0.5f, 0 } + vec3{ i < 2 ? -1 : 1, 0, i % 2 == 0 ? -1 : 1 } *2.5f;
            transform.scale = vec3{ 5,0.1,5 };
            transform.UpdateMatrices();
            ors.AddInstance(model_id, registry, cube, { stone_material });
        }
    }
    {
        auto model_id = render::ModelSystem::GetUnitCube();
        auto cube = registry.create();
        auto &transform = registry.emplace<TransformComponent>(cube);
        transform.position = vec3{ -5, 4.5f, 0 };
        transform.scale = vec3{ 10,0.1,10 };
        transform.rotation = QuaternionFromEuler(0.0f, 0.0f, radians(90.0f));
        transform.UpdateMatrices();
        ors.AddInstance(model_id, registry, cube, { stone_material });
    }
    // ------------------------- LIGHTS -------------------------
    if (true) {
        auto model_id = render::ModelSystem::GetUnitSphereFlat();
        auto entity = registry.create();
        auto &transform = registry.emplace<TransformComponent>(entity);
        transform.scale = vec3{ 0.15f };
        transform.position = vec3{ 0, 3, -2 };
        transform.UpdateMatrices();
        auto &point_light = registry.emplace<PointLight>(entity);
        point_light.color = vec3{ 0.988, 0.933, 0.655 };
        point_light.power = 1e3f;
        ers.AddInstance(model_id, registry, entity, { render::EmissiveMaterial(point_light.color, point_light.power) });
    }
    if (true) {
        auto model_id = render::ModelSystem::GetUnitSphereFlat();
        auto entity = registry.create();
        auto &transform = registry.emplace<TransformComponent>(entity);
        transform.scale = vec3{ 0.15f };
        transform.position = vec3{ 0, 3, 2 };
        transform.UpdateMatrices();
        auto &point_light = registry.emplace<PointLight>(entity);
        point_light.color = vec3{ 0.988, 0.233, 0.255 };
        point_light.power = 2.5e3f;
        ers.AddInstance(model_id, registry, entity, { render::EmissiveMaterial(point_light.color, point_light.power) });
    }
    if (true) {
        auto model_id = render::ModelSystem::GetUnitSphereFlat();
        auto entity = registry.create();
        auto &transform = registry.emplace<TransformComponent>(entity);
        transform.scale = vec3{ 0.15f };
        transform.position = vec3{ 0, 2, -8 };
        transform.UpdateMatrices();
        auto &point_light = registry.emplace<PointLight>(entity);
        point_light.color = vec3{ 0.01, 0.933, 0.255 };
        point_light.power = 3e3f;
        ers.AddInstance(model_id, registry, entity, { render::EmissiveMaterial(point_light.color, point_light.power) });
    }

    // add directional light
    if (true) {
        auto model_id = render::ModelSystem::GetUnitSphereFlat();
        auto entity = registry.create();
        auto &transform = registry.emplace<TransformComponent>(entity);
        transform.scale = vec3{ 0.15f };
        transform.position = vec3{ 0, 10, 0 };
        transform.rotation = QuaternionFromEuler(vec3{ radians(-120.0f), radians(-30.0f), radians(-60.0f) });
        transform.UpdateMatrices();
        auto &directional_light = registry.emplace<DirectionalLight>(entity);
        directional_light.color = vec3{ 0.988, 0.933, 0.455 };
        directional_light.power = 10;
        directional_light.solid_angle = 1.0f;
        ers.AddInstance(model_id, registry, entity, { render::EmissiveMaterial(directional_light.color, directional_light.power) });
    }

    SkyboxManager::LoadSkybox(registry, std::filesystem::current_path() / "assets/textures/skyboxes/night_street/night_street.dds");
    first_scene->OnInstancesUpdated();
    camera_movement::RegisterKeyCallbacks();
    object_editor::RegisterKeyCallbacks();
    auto &input = *InputLayer::instance();
    auto &exposure = hdr_render_pipeline_->hdr_to_ldr_layer()->exposure();
    input.AddTickKeyCallback({ Key::KEY_PLUS }, [this, &exposure] (float dt, InputLayer::KeySeq const &, uint32_t) { exposure += dt; });
    input.AddTickKeyCallback({ Key::KEY_MINUS }, [this, &exposure] (float dt, InputLayer::KeySeq const &, uint32_t) { exposure -= dt; });
    input.AddTickKeyCallback({ Key::KEY_NUMPAD_MINUS }, [this, &exposure] (float dt, InputLayer::KeySeq const &, uint32_t) { exposure -= dt; });
    input.AddTickKeyCallback({ Key::KEY_NUMPAD_PLUS }, [this, &exposure] (float dt, InputLayer::KeySeq const &, uint32_t) { exposure += dt; });
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
    //    Engine::scene()->renderer->light_render_system().RenderShadowMaps(Engine::scene().get());
}