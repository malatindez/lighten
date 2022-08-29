#include "transform-editor.hpp"

using namespace engine;
using namespace core;
using namespace events;
using namespace math;
using namespace components;

namespace transform_editor
{
    std::shared_ptr<engine::core::Scene> selected_scene = nullptr;
    entt::entity selected_entity = entt::null;
    float selected_distance = 0.0f;
    ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
    ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);


    void EditTransform(CameraController const &camera, TransformComponent &transform, AABB const &bounding_box, bool enabled, ivec2 const &window_pos, ivec2 const &window_size)
    {
        mat4 &matrix = transform.model;
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
        ImGuizmo::SetRect((float)window_pos.x, (float)window_pos.y, (float)window_size.x, (float)window_size.y);
        if (enabled)
        {
            static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
            ImGuizmo::Manipulate(camera.camera().view.arr.data(),
                                 camera.camera().projection.arr.data(),
                                 mCurrentGizmoOperation,
                                 mCurrentGizmoMode,
                                 matrix.arr.data(),
                                 NULL,
                                 useSnap ? &snap.x : NULL, bounding_box.min.data.data(), boundsSnap);
            ImGuizmo::DecomposeMatrixToComponents(matrix.arr.data(), matrixTranslation, matrixRotation, matrixScale);
            transform.position = vec3{ matrixTranslation[0], matrixTranslation[1], matrixTranslation[2] };
            transform.rotation = QuaternionFromEuler(matrixRotation[0], matrixRotation[1], matrixRotation[2]);
            transform.scale = vec3{ matrixScale[0], matrixScale[1], matrixScale[2] };
            transform.inv_model = inverse(transform.model);
        }
    }

    void RegisterKeyCallbacks()
    {
        auto &input = *InputLayer::instance();
        auto scene = Engine::scene();
        input.AddUpdateKeyCallback(
            InputLayer::KeySeq{ engine::core::Key::KEY_LBUTTON },
            [&] (InputLayer::KeySeq const &, uint32_t)
            {
                if (ImGui::GetIO().WantCaptureMouse) { return; }
                auto &input = *InputLayer::instance();
                auto scene = Engine::scene();
                Ray ray = scene->main_camera->PixelRaycast(vec2{ input.mouse_position() });
                Intersection nearest;
                nearest.reset();
                std::optional<entt::entity> entity = render::ModelSystem::FindIntersection(scene->registry, ray, nearest);
                if (entity.has_value())
                {
                    selected_entity = entity.value();
                    selected_scene = scene;
                    selected_distance = nearest.t;
                }
                else
                {
                    selected_entity = entt::null;
                    selected_scene = nullptr;
                    selected_distance = 0;
                }
            },
            false);
        input.AddUpdateKeyCallback(
            InputLayer::KeySeq{ engine::core::Key::KEY_R },
            [&] (InputLayer::KeySeq const &, uint32_t count)
            {
                if (count == std::numeric_limits<uint32_t>::max()) { return; }
                mCurrentGizmoOperation = ImGuizmo::ROTATE;
            },
            false);
        input.AddUpdateKeyCallback(
            InputLayer::KeySeq{ engine::core::Key::KEY_T },
            [&] (InputLayer::KeySeq const &, uint32_t count)
            {
                if (count == std::numeric_limits<uint32_t>::max()) { return; }
                mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
            },
            false);
        input.AddUpdateKeyCallback(
            InputLayer::KeySeq{ engine::core::Key::KEY_F },
            [&] (InputLayer::KeySeq const &, uint32_t count)
            {
                if (count == std::numeric_limits<uint32_t>::max()) { return; }
                mCurrentGizmoOperation = ImGuizmo::SCALE;
            },
            false);

    }

    void OnGuiRender(ivec2 const &window_pos, ivec2 const &window_size)
    {
        static TransformComponent empty;
        static AABB empty_bb;
        if (selected_entity != entt::null && selected_scene == Engine::scene())
        {
            TransformComponent &transform = Engine::scene()->registry.get<TransformComponent>(selected_entity);
            auto id = Engine::scene()->registry.get<ModelComponent>(selected_entity).model_id;
            auto const &model_ref = render::ModelSystem::GetModel(id);
            EditTransform(*Engine::scene()->main_camera, transform, model_ref.bounding_box, true, window_pos, window_size);
            render::ModelSystem::instance()->OnInstancesUpdated(Engine::scene()->registry);
        }
        else
        {
            ImGui::BeginDisabled();
            EditTransform(*Engine::scene()->main_camera, empty, empty_bb, false, window_pos, window_size);
            ImGui::EndDisabled();
        }
    }

}