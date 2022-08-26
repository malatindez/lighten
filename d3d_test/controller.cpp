#include "controller.hpp"
using namespace engine;
using namespace core;
using namespace events;
using namespace math;
using namespace components;
void EditTransform(CameraController const &camera, mat4 &matrix, bool enabled = true)
{
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
    if (ImGui::IsKeyPressed(90))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(69))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(82)) // r Key
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    ImGuizmo::DecomposeMatrixToComponents(matrix.arr.data(), matrixTranslation, matrixRotation, matrixScale);
    ImGui::InputFloat3("Tr", matrixTranslation, "%.3f", 3);
    ImGui::InputFloat3("Rt", matrixRotation, "%.3f", 3);
    ImGui::InputFloat3("Sc", matrixScale, "%.3f", 3);
    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix.arr.data());

    if (mCurrentGizmoOperation != ImGuizmo::SCALE)
    {
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
            mCurrentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
            mCurrentGizmoMode = ImGuizmo::WORLD;
    }
    static bool useSnap(false);
    if (ImGui::IsKeyPressed(83))
        useSnap = !useSnap;
    ImGui::Checkbox("checkbox", &useSnap);
    ImGui::SameLine();
    vec4 snap;
    switch (mCurrentGizmoOperation)
    {
    case ImGuizmo::TRANSLATE:
        ImGui::InputFloat3("Snap", &snap.x);
        break;
    case ImGuizmo::ROTATE:
        ImGui::InputFloat("Angle Snap", &snap.x);
        break;
    case ImGuizmo::SCALE:
        ImGui::InputFloat("Scale Snap", &snap.x);
        break;
    }
    ImGuiIO &io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    if (enabled)
    {
        ImGuizmo::Manipulate(camera.camera().view.arr.data(),
                             camera.camera().projection.arr.data(),
                             mCurrentGizmoOperation,
                             mCurrentGizmoMode,
                             matrix.arr.data(),
                             NULL,
                             useSnap ? &snap.x : NULL);
    }
}

void Controller::OnGuiRender()
{
    static bool b = true;
    static mat4 empty{ 1 };
    ImGui::Begin("Framerate");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::Text("Move speed: ");
    ImGui::Text("%.3f", first_scene->main_camera->move_speed());
    ImGui::Text("FOV: ");
    ImGui::Text("%.3f", first_scene->main_camera->fovy());
    ImGui::Text("Flags: ");
    ImGui::Text("%u", first_scene->main_camera->flags());
    ImGui::Text("Edit target transform: ");
    if (selected_entity_ != entt::null && selected_entity_scene_ == Engine::scene())
    {
        TransformComponent &transform = Engine::scene()->registry.get<TransformComponent>(selected_entity_);
        EditTransform(*Engine::scene()->main_camera, transform.model);
        render::ModelSystem::instance()->OnInstancesUpdated(Engine::scene()->registry);
    }
    else
    {
        ImGui::BeginDisabled();
        EditTransform(*Engine::scene()->main_camera, empty, false);
        ImGui::EndDisabled();
    }
    ImGui::End();

    //ImGui::Begin("Transform Editor");
}

Controller::Controller(std::shared_ptr<Renderer> renderer, math::ivec2 const &window_size) : current_mouse_position_(InputLayer::instance()->mouse_position()), renderer_{ renderer }
{
    first_scene = std::make_shared<Scene>();

    auto &registry = first_scene->registry;
    main_camera_entity = registry.create();
    registry.emplace<CameraComponent>(main_camera_entity, CameraComponent());
    registry.emplace<TransformComponent>(main_camera_entity, TransformComponent());
    registry.emplace<TagComponent>(main_camera_entity, TagComponent{ .tag = "Main Camera" });
    first_scene->main_camera = std::make_unique<CameraController>(&registry, main_camera_entity, window_size);
    Engine::SetScene(first_scene);
    int amount = 10;
    for (int i = 0; i < amount; i++)
    {
        auto knight = registry.create();
        last_created_knight = knight;
        auto &transform = registry.emplace<TransformComponent>(knight);
        registry.emplace<OpaqueComponent>(knight);
        registry.emplace<ModelComponent>(knight).model_id = renderer->knight_model_id;
        transform.position = vec3
        {
            std::sin(float(i) / amount * 2 * (float)std::numbers::pi),
            0,
            std::cos(float(i) / amount * 2 * (float)std::numbers::pi)
        } *(float(amount) / std::sqrtf(amount));
        transform.rotation = QuaternionFromEuler(radians(180.0f), 0.0f, radians(180.0f));
        transform.rotation *= QuaternionFromRotationMatrix(lookAt(transform.position, vec3{ 0,0,0 }, vec3{ 0,1,0 }).as_rmat<3, 3>());
        transform.UpdateMatrices();
    }
    render::ModelSystem::instance()->OnInstancesUpdated(registry);
    auto input = InputLayer::instance();

    input->AddUpdateKeyCallback(
        InputLayer::KeySeq{ engine::core::Key::KEY_W },
        [this] (InputLayer::KeySeq const &, uint32_t)
        { first_scene->main_camera->flags() |= CameraController::MoveForward; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq{ engine::core::Key::KEY_S },
        [this] (InputLayer::KeySeq const &, uint32_t)
        { first_scene->main_camera->flags() |= CameraController::MoveBackwards; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq{ engine::core::Key::KEY_A },
        [this] (InputLayer::KeySeq const &, uint32_t)
        { first_scene->main_camera->flags() |= CameraController::MoveLeft; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq{ engine::core::Key::KEY_D },
        [this] (InputLayer::KeySeq const &, uint32_t)
        { first_scene->main_camera->flags() |= CameraController::MoveRight; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq{ engine::core::Key::KEY_SPACE },
        [this] (InputLayer::KeySeq const &, uint32_t)
        { first_scene->main_camera->flags() |= CameraController::MoveUp; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq{ engine::core::Key::KEY_CONTROL },
        [this] (InputLayer::KeySeq const &, uint32_t)
        { first_scene->main_camera->flags() |= CameraController::MoveDown; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq{ engine::core::Key::KEY_Q },
        [this] (InputLayer::KeySeq const &, uint32_t)
        { first_scene->main_camera->flags() |= CameraController::RotateLeft; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq{ engine::core::Key::KEY_E },
        [this] (InputLayer::KeySeq const &, uint32_t)
        { first_scene->main_camera->flags() |= CameraController::RotateRight; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq{ engine::core::Key::KEY_SHIFT },
        [this] (InputLayer::KeySeq const &, uint32_t)
        { first_scene->main_camera->flags() |= CameraController::Accelerate; });
    input->AddUpdateKeyCallback(
        InputLayer::KeySeq{ engine::core::Key::KEY_RBUTTON },
        [this] (InputLayer::KeySeq const &, uint32_t count)
        {
            auto &input = *InputLayer::instance();
            auto scene = Engine::scene();
            auto &registry = scene->registry;
            Ray ray = scene->main_camera->PixelRaycast(vec2{ input.mouse_position() });
            Intersection nearest;
            nearest.reset();
            std::optional<entt::entity> entity = render::ModelSystem::FindIntersection(scene->registry, ray, nearest);
            if (entity.has_value())
            {
                selected_entity_ = entity.value();
                selected_entity_scene_ = scene;
                TransformComponent &transform = registry.get<TransformComponent>(selected_entity_);
                selected_object_distance_ = nearest.t;
                selected_object_offset_ = transform.position - ray.PointAtParameter(nearest.t);
            }
            else
            {
                selected_entity_ = entt::null;
                selected_entity_scene_ = nullptr;
                selected_object_distance_ = 0;
            }
        }, false);
}
void Controller::OnTick([[maybe_unused]] float delta_time)
{
    for (auto const &func : update_callbacks_)
    {
        func(delta_time);
    }
    ivec2 pixel_delta{ 0 };

    auto &input = *InputLayer::instance();
    auto scene = Engine::scene();

    if (input.lbutton_down())
    {
        if (previous_mouse_position == core::math::vec2{ -1, -1 })
        {
            previous_mouse_position = current_mouse_position_;
        }
        pixel_delta = current_mouse_position_ - previous_mouse_position;
    }
    else
    {
        previous_mouse_position = core::math::ivec2{ -1, -1 };
    }
    first_scene->main_camera->OnTick(delta_time, pixel_delta);

    renderer_->per_frame.view_projection = first_scene->main_camera->camera().view_projection;

    if (input.mouse_scrolled())
    {
        if (selected_entity_ != entt::null && selected_entity_scene_ == scene && input.rbutton_down())
        {
            TransformComponent &transform = scene->registry.get<TransformComponent>(selected_entity_);
            transform.scale *= vec3{ powf(math::clamp(1 + delta_time / 120 * input.scroll_delta(), 0.5f, 1.5f), 0.5f) };
            rclamp(transform.scale, 0.1f, std::numeric_limits<float>::max());
        }
        else
        {
            auto &move_speed = scene->main_camera->move_speed();
            move_speed = std::max(0.01f, move_speed * (input.scroll_delta() > 0 ? 1.1f : 1.0f / 1.1f));
        }
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