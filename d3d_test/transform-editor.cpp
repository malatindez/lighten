#include "transform-editor.hpp"

using namespace engine;
using namespace core;
using namespace events;
using namespace math;
using namespace components;

namespace object_editor
{
    std::shared_ptr<engine::core::Scene> selected_scene = nullptr;
    entt::entity selected_entity = entt::null;
    render::Mesh const *selected_mesh = nullptr;
    float selected_distance = 0.0f;
    ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
    ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

    void EditTransform(CameraController const &camera, TransformComponent &transform, Box const &bounding_box, bool enabled, ivec2 const &window_pos, ivec2 const &window_size)
    {
        static bool useSnap(false);
        mat4 &matrix = transform.model;
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(matrix.arr.data(), matrixTranslation, matrixRotation, matrixScale);
        vec4 snap;
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_SpanAvailWidth))
        {
            if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
                mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
            ImGui::SameLine();
            if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
                mCurrentGizmoOperation = ImGuizmo::ROTATE;
            ImGui::SameLine();
            if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
                mCurrentGizmoOperation = ImGuizmo::SCALE;
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
            if (ImGui::IsKeyPressed(83))
                useSnap = !useSnap;
            ImGui::Checkbox("checkbox", &useSnap);
            ImGui::SameLine();
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
            default:
                utils::AlwaysAssert(false);
                break;
            }
            if (ImGui::Button("Reset"))
            {
                matrix = mat4::identity();
                ImGuizmo::DecomposeMatrixToComponents(matrix.arr.data(), matrixTranslation, matrixRotation, matrixScale);
            }
            if (ImGui::Button("Reset scale"))
            {
                matrixScale[0] = matrixScale[1] = matrixScale[2] = 1;
                ImGuizmo::DecomposeMatrixToComponents(matrix.arr.data(), matrixTranslation, matrixRotation, matrixScale);
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset rotation"))
            {
                matrixRotation[0] = matrixRotation[1] = matrixRotation[2] = 0;
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset rotation and scale"))
            {
                matrixScale[0] = matrixScale[1] = matrixScale[2] = 1;
                matrixRotation[0] = matrixRotation[1] = matrixRotation[2] = 0;
            }
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
                                 nullptr,
                                 useSnap ? &snap.x : nullptr, Box{ .min = -bounding_box.min, .max = -bounding_box.max }.min.data.data(), boundsSnap);
            ImGuizmo::DecomposeMatrixToComponents(matrix.arr.data(), matrixTranslation, matrixRotation, matrixScale);
        }
        transform.position = vec3{ matrixTranslation[0], matrixTranslation[1], matrixTranslation[2] };
        transform.scale = vec3{ matrixScale[0], matrixScale[1], matrixScale[2] };
        transform.rotation = QuaternionFromRotationMatrix(scale(matrix, 1.0f / transform.scale).as_rmat<3, 3>());
        transform.UpdateMatrices();
    }
    void EditTransform(ivec2 const &window_pos, ivec2 const &window_size)
    {
        static TransformComponent empty;
        if (selected_entity != entt::null && selected_scene == Engine::scene())
        {
            TransformComponent &transform = Engine::scene()->registry.get<TransformComponent>(selected_entity);
            uint64_t id = 0;

            if (auto const *opaque = Engine::scene()->registry.try_get<OpaqueComponent>(selected_entity);
                opaque != nullptr)
            {
                id = opaque->model_id;
            }
            else if (auto const *emissive = Engine::scene()->registry.try_get<EmissiveComponent>(selected_entity);
                     emissive != nullptr)
            {
                id = emissive->model_id;
            }
            auto const &model = render::ModelSystem::GetModel(id);
            EditTransform(*Engine::scene()->main_camera, transform, model.bounding_box, InputLayer::instance()->key_state(engine::core::Key::KEY_CONTROL), window_pos, window_size);
            Engine::scene()->OnInstancesUpdated();
        }
        else
        {
            ImGui::BeginDisabled();
            EditTransform(*Engine::scene()->main_camera, empty, Box::empty(), false, window_pos, window_size);
            ImGui::EndDisabled();
        }
    }
    void EditMaterialEmpty()
    {
        ImGui::BeginDisabled();
        ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_SpanAvailWidth);
        ImGui::EndDisabled();
    }

    void EditMaterialOpaque(render::OpaqueMaterial *material)
    {
        if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_SpanAvailWidth))
        {
            static entt::entity entity;
            static uint64_t albedo_map_texture_id = 0;
            static uint64_t saved_albedo_map_texture_id = 0;
            static bool albedo_map_texture_enabled = false;
            static uint64_t normal_map_texture_id = 0;
            static uint64_t saved_normal_map_texture_id = 0;
            static bool normal_map_texture_enabled = false;
            static uint64_t roughness_map_texture_id = 0;
            static uint64_t saved_roughness_map_texture_id = 0;
            static bool roughness_map_texture_enabled = false;
            static uint64_t metallic_map_texture_id = 0;
            static uint64_t saved_metallic_map_texture_id = 0;
            static bool metallic_map_texture_enabled = false;
            if (entity != selected_entity)
            {
                entity = selected_entity;
                saved_albedo_map_texture_id = albedo_map_texture_id = TextureManager::GetTextureIdByPointer(material->albedo_map);
                saved_normal_map_texture_id = normal_map_texture_id = TextureManager::GetTextureIdByPointer(material->normal_map);
                saved_roughness_map_texture_id = roughness_map_texture_id = TextureManager::GetTextureIdByPointer(material->roughness_map);
                saved_metallic_map_texture_id = metallic_map_texture_id = TextureManager::GetTextureIdByPointer(material->metalness_map);
                albedo_map_texture_enabled = albedo_map_texture_id != kInvalidTextureId;
                normal_map_texture_enabled = normal_map_texture_id != kInvalidTextureId;
                roughness_map_texture_enabled = roughness_map_texture_id != kInvalidTextureId;
                metallic_map_texture_enabled = metallic_map_texture_id != kInvalidTextureId;
            }
            ImGui::Checkbox("##albedo_map_enabled", &albedo_map_texture_enabled);
            ImGui::SameLine();
            ImGui::BeginDisabled(!albedo_map_texture_enabled);
            ImGui::InputScalar("Albedo map texture ID", ImGuiDataType_U64, &albedo_map_texture_id, nullptr, nullptr, "%llu");
            ImGui::EndDisabled();
            ImGui::Checkbox("##normal_map_enabled", &normal_map_texture_enabled);
            ImGui::SameLine();
            ImGui::BeginDisabled(!normal_map_texture_enabled);
            ImGui::InputScalar("Normal map texture ID", ImGuiDataType_U64, &normal_map_texture_id, nullptr, nullptr, "%llu");
            ImGui::EndDisabled();
            ImGui::Checkbox("##roughness_map_enabled", &roughness_map_texture_enabled);
            ImGui::SameLine();
            ImGui::BeginDisabled(!roughness_map_texture_enabled);
            ImGui::InputScalar("Roughness map texture ID", ImGuiDataType_U64, &roughness_map_texture_id, nullptr, nullptr, "%llu");
            ImGui::EndDisabled();
            ImGui::Checkbox("##metallic_map_enabled", &metallic_map_texture_enabled);
            ImGui::SameLine();
            ImGui::BeginDisabled(!metallic_map_texture_enabled);
            ImGui::InputScalar("Metallic map texture ID", ImGuiDataType_U64, &metallic_map_texture_id, nullptr, nullptr, "%llu");
            ImGui::EndDisabled();
            ImGui::ColorEdit3("Albedo color", material->albedo_color.data.data());
            ImGui::SliderFloat("Metalness", &material->metalness_value, 0.001f, 1.0f);
            ImGui::SliderFloat("Roughness", &material->roughness_value, 0.001f, 1.0f);
            auto width = ::ImGui::GetContentRegionMax().x - 200;
            ImGui::PushItemWidth(width / 2);
            ImGui::SliderFloat("X", &material->uv_multiplier.x, 1, 100);
            ImGui::SameLine();
            ImGui::PushItemWidth(width / 2);
            ImGui::SliderFloat("Y", &material->uv_multiplier.y, 1, 100);
            ImGui::SameLine();
            ImGui::Text("UV multiplier");
            ImGui::Checkbox("Reverse normal y", &material->reverse_normal_y);
            if (!albedo_map_texture_enabled)
                albedo_map_texture_id = kInvalidTextureId;
            if (!normal_map_texture_enabled)
                normal_map_texture_id = kInvalidTextureId;
            if (!roughness_map_texture_enabled)
                roughness_map_texture_id = kInvalidTextureId;
            if (!metallic_map_texture_enabled)
                metallic_map_texture_id = kInvalidTextureId;
            if (saved_albedo_map_texture_id != albedo_map_texture_id)
            {
                material->albedo_map = TextureManager::GetTextureView(albedo_map_texture_id);
                saved_albedo_map_texture_id = albedo_map_texture_id;
            }
            if (saved_normal_map_texture_id != normal_map_texture_id)
            {
                material->normal_map = TextureManager::GetTextureView(normal_map_texture_id);
                saved_normal_map_texture_id = normal_map_texture_id;
            }
            if (saved_roughness_map_texture_id != roughness_map_texture_id)
            {
                material->roughness_map = TextureManager::GetTextureView(roughness_map_texture_id);
                saved_roughness_map_texture_id = roughness_map_texture_id;
            }
            if (saved_metallic_map_texture_id != metallic_map_texture_id)
            {
                material->metalness_map = TextureManager::GetTextureView(metallic_map_texture_id);
                saved_metallic_map_texture_id = metallic_map_texture_id;
            }
            material->UpdateTextureFlags();
        }
    }

    void EditMaterialEmissive(render::EmissiveMaterial *material)
    {
        if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_SpanAvailWidth))
        {
            static entt::entity entity;
            static uint64_t texture_id = 0;
            static uint64_t saved_texture_id = 0;
            static bool texture_enabled = false;
            if (entity != selected_entity)
            {
                entity = selected_entity;
                texture_id = TextureManager::GetTextureIdByPointer(material->emissive_texture);
                saved_texture_id = TextureManager::GetTextureIdByPointer(material->emissive_texture);
                texture_enabled = texture_id == kInvalidTextureId;
            }

            ImGui::Checkbox("##ambient_enabled", &texture_enabled);
            ImGui::SameLine();
            ImGui::BeginDisabled(!texture_enabled);
            ImGui::InputScalar("Texture ID", ImGuiDataType_U64, &texture_id, nullptr, nullptr, "%llu");
            ImGui::EndDisabled();
            ImGui::ColorEdit3("Emissive color", material->emissive_color.data.data());
            ImGui::SliderFloat("Power", &material->power, 0, 1e6f);
            if (!texture_enabled)
                texture_id = kInvalidTextureId;
            if (saved_texture_id != texture_id)
            {
                material->emissive_texture = TextureManager::GetTextureView(texture_id);
                saved_texture_id = texture_id;
            }
        }
    }
    void EditMaterial()
    {
        auto &scene = *Engine::scene();
        auto &registry = scene.registry;
        if (selected_entity == entt::null)
        {
            EditMaterialEmpty();
            return;
        }
        bool flag = false;
        if (auto *component = registry.try_get<components::OpaqueComponent>(selected_entity); component)
        {
            auto *model_instance = Engine::scene()->renderer->opaque_render_system().GetInstancePtr(component->model_id);
            if (model_instance)
            {
                uint32_t mesh_id = std::numeric_limits<uint32_t>::max();
                for (size_t i = 0; i < model_instance->model.meshes.size(); i++)
                {
                    if (&model_instance->model.meshes[i].mesh == selected_mesh)
                    {
                        mesh_id = (uint32_t)i;
                    }
                }
                if (mesh_id != std::numeric_limits<uint32_t>::max())
                {
                    for (auto &material_instance : model_instance->mesh_instances[mesh_id].material_instances)
                    {
                        if (auto it = std::ranges::find(material_instance.instances, selected_entity); it != material_instance.instances.end())
                        {
                            EditMaterialOpaque(&material_instance.material);
                            flag = true;
                            break;
                        }
                    }
                }
            }
        }
        else if (auto *component = registry.try_get<components::EmissiveComponent>(selected_entity); component)
        {
            auto *model_instance = Engine::scene()->renderer->emissive_render_system().GetInstancePtr(component->model_id);
            if (model_instance)
            {
                uint32_t mesh_id = std::numeric_limits<uint32_t>::max();
                for (size_t i = 0; i < model_instance->model.meshes.size(); i++)
                {
                    if (&model_instance->model.meshes[i].mesh == selected_mesh)
                    {
                        mesh_id = (uint32_t)i;
                    }
                }
                if (mesh_id != std::numeric_limits<uint32_t>::max())
                {
                    for (auto &material_instance : model_instance->mesh_instances[mesh_id].material_instances)
                    {
                        if (auto it = std::ranges::find(material_instance.instances, selected_entity); it != material_instance.instances.end())
                        {
                            EditMaterialEmissive(&material_instance.material);
                            flag = true;
                            break;
                        }
                    }
                }
            }
        }
        if (!flag)
        {
            EditMaterialEmpty();
        }
    }
    void EditLightEmpty()
    {
        ImGui::BeginDisabled();
        ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_SpanAvailWidth);
        ImGui::EndDisabled();
    }
    void EditLightPoint(components::PointLight *point_light)
    {
        if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_SpanAvailWidth))
        {
            ImGui::Text("Point light");
            ImGui::ColorEdit3("Color", point_light->color.data.data());
            ImGui::SliderFloat("Power", &point_light->power, 0, 1e6f);
        }
    }
    void EditLightSpot(components::SpotLight *spot_light)
    {
        if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_SpanAvailWidth))
        {
            ImGui::Text("Spot light");
            ImGui::ColorEdit3("Color", spot_light->color.data.data());
            ImGui::SliderFloat("Power", &spot_light->power, 0, 1e6f);
            ImGui::InputFloat3("Direction", spot_light->direction.data.data());
            ImGui::SliderFloat("Cut off", &spot_light->cut_off, -std::numbers::pi, std::numbers::pi);
        }
    }
    void EditLightDirectional(components::DirectionalLight *directional_light)
    {
        if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_SpanAvailWidth))
        {
            ImGui::Text("Directional light");
            ImGui::ColorEdit3("Color", directional_light->color.data.data());
            ImGui::SliderFloat("Power", &directional_light->power, 0, 1e6f);
            ImGui::InputFloat("Solid angle", &directional_light->solid_angle);
        }
    }
    void EditLight()
    {
        auto &scene = *Engine::scene();
        auto &registry = scene.registry;
        if (selected_entity == entt::null)
        {
            EditLightEmpty();
            return;
        }
        if (auto *light = registry.try_get<components::PointLight>(selected_entity); light)
        {
            return EditLightPoint(light);
        }
        if (auto *light = registry.try_get<components::SpotLight>(selected_entity); light)
        {
            return EditLightSpot(light);
        }
        if (auto *light = registry.try_get<components::DirectionalLight>(selected_entity); light)
        {
            return EditLightDirectional(light);
        }
    }
    void ShowTextureList()
    {
        if (ImGui::BeginTable("TextureList", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY))
        {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Id", ImGuiTableColumnFlags_WidthFixed, 50.0f);
            ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();
            for (auto const &[texture_id, ptr] : core::TextureManager::GetTextures())
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%llu", texture_id);
                ImGui::TableNextColumn();
                auto &texture_paths = core::TextureManager::GetTexturePaths();
                if (texture_paths.contains(texture_id))
                {
                    ImGui::Text("%s", core::TextureManager::GetTexturePaths().at(texture_id).string().c_str());
                }
                else
                {
                    ImGui::Text("Unknown");
                }
            }
            ImGui::EndTable();
        }
    }
    void OnGuiRender(ivec2 const &window_pos, ivec2 const &window_size)
    {
        ImGui::Begin("Component editor");
        EditTransform(window_pos, window_size);
        ImGui::Spacing();
        EditMaterial();
        ImGui::Spacing();
        EditLight();
        ImGui::End();
        ImGui::Begin("Texture list");
        ShowTextureList();
        ImGui::End();
    }
    void RegisterKeyCallbacks()
    {
        auto &input = *InputLayer::instance();
        auto scene = Engine::scene();
        input.AddUpdateKeyCallback(
            InputLayer::KeySeq{ engine::core::Key::KEY_CONTROL, engine::core::Key::KEY_LBUTTON },
            [&] (InputLayer::KeySeq const &, uint32_t)
            {
                if (ImGui::GetIO().WantCaptureMouse)
                {
                    return;
                }
                auto &input = *InputLayer::instance();
                auto scene = Engine::scene();
                Ray ray = scene->main_camera->PixelRaycast(vec2{ input.mouse_position() });
                MeshIntersection nearest;
                nearest.reset();
                std::optional<entt::entity> entity = render::ModelSystem::FindIntersection(scene->registry, ray, nearest);
                if (entity.has_value())
                {
                    selected_entity = entity.value();
                    selected_scene = scene;
                    selected_distance = nearest.t;
                    selected_mesh = nearest.mesh_ptr;
                }
                else
                {
                    selected_entity = entt::null;
                    selected_scene = nullptr;
                    selected_distance = 0;
                    selected_mesh = nullptr;
                }
            },
            false);
        input.AddUpdateKeyCallback(
            InputLayer::KeySeq{ engine::core::Key::KEY_R },
            [&] (InputLayer::KeySeq const &, uint32_t count)
            {
                if (count == std::numeric_limits<uint32_t>::max())
                {
                    return;
                }
                mCurrentGizmoOperation = ImGuizmo::ROTATE;
            },
            false);
        input.AddUpdateKeyCallback(
            InputLayer::KeySeq{ engine::core::Key::KEY_T },
            [&] (InputLayer::KeySeq const &, uint32_t count)
            {
                if (count == std::numeric_limits<uint32_t>::max())
                {
                    return;
                }
                mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
            },
            false);
        input.AddUpdateKeyCallback(
            InputLayer::KeySeq{ engine::core::Key::KEY_F },
            [&] (InputLayer::KeySeq const &, uint32_t count)
            {
                if (count == std::numeric_limits<uint32_t>::max())
                {
                    return;
                }
                mCurrentGizmoOperation = ImGuizmo::SCALE;
            },
            false);

        input.AddUpdateKeyCallback(
            InputLayer::KeySeq{ engine::core::Key::KEY_C },
            [&] (InputLayer::KeySeq const &, uint32_t count)
            {
                if (count == std::numeric_limits<uint32_t>::max())
                {
                    return;
                }
                if (ImGuizmo::LOCAL == mCurrentGizmoMode)
                {
                    mCurrentGizmoMode = ImGuizmo::WORLD;
                }
                else
                {
                    mCurrentGizmoMode = ImGuizmo::LOCAL;
                }
            },
            false);
    }
}