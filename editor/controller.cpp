#include "controller.hpp"
#include "camera-movement.hpp"
#include "reflection/reflection.hpp"
#include "gui/scene-viewer.hpp"

using namespace lighten;
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

    ImGui::Text("Average Engine::Update() time: %.3f ms", 1000ull * update_measurer.avg());
    ImGui::Text("Average Engine::Update() time over the last %d calls: %.3f ms", calls_amount, 1000ull * update_measurer.avg(calls_amount));
    ImGui::Text("Average Engine::Update() time over the last %.2f seconds: %.3f ms", seconds_amount, 1000ull * update_measurer.avg_over_the_last(seconds_amount));
    ImGui::Text("Average Engine::Update() time percentage over the last %.2f seconds: %.3f%%", seconds_amount, update_measurer.avg_over_the_last_limited(seconds_amount) * 100ull);
    ImGui::Text("Average Engine::Update() amount of calls over the last %.2f seconds: %llu", std::min(seconds_amount, update_measurer.elapsed()), update_measurer.amount_of_calls(seconds_amount));

    ImGui::Text("Average Engine::Tick() time: %.3f ms", 1000ull * tick_measurer.avg());
    ImGui::Text("Average Engine::Tick() time over the last %d calls: %.3f ms", calls_amount, 1000ull * tick_measurer.avg(calls_amount));
    ImGui::Text("Average Engine::Tick() time over the last %.2f seconds: %.3f ms", seconds_amount, 1000ull * tick_measurer.avg_over_the_last(seconds_amount));
    ImGui::Text("Average Engine::Tick() time percentage over the last %.2f seconds: %.3f%%", seconds_amount, tick_measurer.avg_over_the_last_limited(seconds_amount) * 100ull);
    ImGui::Text("Average Engine::Tick() amount of calls over the last %.2f seconds: %llu", std::min(seconds_amount, tick_measurer.elapsed()), tick_measurer.amount_of_calls(seconds_amount));

    ImGui::Text("Average Engine::Render() time: %.3f ms", 1000ull * render_measurer.avg());
    ImGui::Text("Average Engine::Render() time over the last %d calls: %.3f ms", calls_amount, 1000ull * render_measurer.avg(calls_amount));
    ImGui::Text("Average Engine::Render() time over the last %.2f seconds: %.3f ms", seconds_amount, 1000ull * render_measurer.avg_over_the_last(seconds_amount));
    ImGui::Text("Average Engine::Render() time percentage over the last %.2f seconds: %.3f%%", seconds_amount, render_measurer.avg_over_the_last_limited(seconds_amount) * 100ull);
    ImGui::Text("Average Engine::Render() amount of calls over the last %.2f seconds: %llu", std::min(seconds_amount, render_measurer.elapsed()), render_measurer.amount_of_calls(seconds_amount));
    ImGui::End();
#endif
    ImGui::Begin("Main settings");
    ImGui::Checkbox("Dynamic shadows: ", &camera_movement::dynamic_shadows);
    ImGui::SliderFloat("Shadows update interval##shadows-update-interval",
                       &Engine::scene()->renderer->light_render_system().shadow_map_update_interval(), 0, 30, "%.3f");
    // make button and text field to set the camera fov
    static float fov = Engine::scene()->main_camera->fovy();
    ImGui::SliderFloat("FOV: ", &fov, 0.0f, glm::radians(180.0f));
    if (Engine::scene()->main_camera->fovy() != fov)
    {
        Engine::scene()->main_camera->fovy() = fov;
        Engine::scene()->main_camera->UpdateProjectionMatrix();
    }
    auto &camera_controller = *Engine::scene()->main_camera;
    ImGui::Checkbox("Roll enabled##roll-enabled", &camera_controller.roll_enabled());
    ImGui::SliderFloat("Sensitivity##sensitivity", &camera_controller.sensitivity(), 0, 100, "%.3f");
    ImGui::SliderFloat("Move speed##move-speed", &camera_controller.move_speed(), 0, 100, "%.3f");
    ImGui::SliderFloat("Accelerated movement speed##accelerated-speed", &camera_controller.accelerated_speed(), 0, 100, "%.3f");
    ImGui::SliderFloat("Roll speed##roll-speed", &camera_controller.roll_speed(), 0, 100, "%.3f");

    auto hdr_to_ldr = hdr_render_pipeline_->hdr_to_ldr_layer();
    ImGui::SliderFloat("Exposure", &hdr_to_ldr->exposure(), -20.0f, 20.0f);
    ImGui::SliderFloat("Gamma", &hdr_to_ldr->gamma(), 0.0f, 20.0f);
    static float update_limit = Engine::maximum_update_rate();
    ImGui::SliderFloat("Update limit", &update_limit, 1.0f, 10000.0f);
    if (update_limit != Engine::maximum_update_rate())
    {
        Engine::SetMaximumUpdateRate(update_limit);
    }
    static float render_limit = Engine::maximum_framerate();
    ImGui::SliderFloat("Render limit", &render_limit, 5.0f, 10000.0f);
    if (render_limit != Engine::maximum_framerate())
    {
        Engine::SetMaximumFramerate(render_limit);
    }
    static float tick_limit = Engine::maximum_tickrate();
    ImGui::SliderFloat("Tick limit", &tick_limit, 1.0f, 10000.0f);
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
    static glm::uvec2 poisson_disk_size = { 0.5f, 0.5f };
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
        auto dots = random::poisson_disc::Generate(poisson_disk_size, k, r);
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
    scene_viewer_->Render();
//    object_editor::OnGuiRender();
//    object_editor::OnRender(hdr_render_pipeline_->window()->position(), hdr_render_pipeline_->framebuffer_size());
    ImGui::ShowDemoWindow();
}

Controller::Controller(std::shared_ptr<direct3d::DeferredHDRRenderPipeline> hdr_render_pipeline)
    : hdr_render_pipeline_{hdr_render_pipeline}
{

    first_scene = std::make_shared<Scene>();
    scene_viewer_ = std::make_unique<gui::SceneViewer>(first_scene->registry);
    first_scene->renderer = std::make_unique<render::Renderer>();

    camera_movement::dynamic_shadows = false;
    first_scene->renderer->light_render_system().shadow_map_update_interval() = 30.0f;

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
    registry.emplace<Camera>(main_camera_entity, Camera());
    registry.emplace<Transform>(main_camera_entity, Transform());
    first_scene->main_camera = std::make_unique<CameraController>(&registry, main_camera_entity, hdr_render_pipeline_->framebuffer_size());
    Engine::SetScene(first_scene);

    // todo: move it to engine itself, figure out a better way to handle registry
    mal_toolkit::constexpr_for < 0, reflection::SystemTuple::amount, 1>([&](auto i) constexpr -> void
        {
            using T = reflection::SystemTuple::type_at<i>::type;
            std::shared_ptr<T> system = std::make_shared<T>(registry);
            // todo push to local array of systems as well
            Engine::Get().PushLayer(system);
        });
    Engine::scene()->main_camera->SetWorldOffset(glm::vec3{0.0f, 0.0f, 0.0f});
    Engine::scene()->main_camera->SetWorldAngles(0.0f, 0.0f, 0.0f);
    Engine::scene()->main_camera->SetProjectionMatrix(glm::perspectiveLH_ZO(45.0f, 16.0f / 9.0f, 0.001f, 100.0f));

    
    SkyboxManager::LoadSkybox(registry, std::filesystem::current_path() / "assets/textures/skyboxes/night_street/night_street.dds");
    first_scene->ScheduleInstanceUpdate();
    camera_movement::RegisterKeyCallbacks();
     auto &input = *InputLayer::instance();
    auto &exposure = hdr_render_pipeline_->hdr_to_ldr_layer()->exposure();
    input.AddTickKeyCallback({Key::KEY_PLUS}, [&exposure](float dt, InputLayer::KeySeq const &, uint32_t)
                             { exposure += dt; });
    input.AddTickKeyCallback({Key::KEY_MINUS}, [&exposure](float dt, InputLayer::KeySeq const &, uint32_t)
                             { exposure -= dt; });
    input.AddTickKeyCallback({Key::KEY_NUMPAD_MINUS}, [&exposure](float dt, InputLayer::KeySeq const &, uint32_t)
                             { exposure -= dt; });
    input.AddTickKeyCallback({Key::KEY_NUMPAD_PLUS}, [&exposure](float dt, InputLayer::KeySeq const &, uint32_t)
                             { exposure += dt; });
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
void Controller::OnEvent(lighten::core::events::Event &e)
{
    if (e.type() == lighten::core::events::EventType::WindowClose) [[unlikely]]
    {
        lighten::core::Engine::Exit();
    }
    else if (e.type() == lighten::core::events::EventType::WindowResize) [[unlikely]]
    {
        Engine::scene()->main_camera->UpdateProjectionMatrix();
    }
}
void Controller::OnUpdate()
{
    camera_movement::OnUpdate();
}